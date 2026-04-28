// Copyright 2025 itlab-vision

#include <algorithm>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

#include "coordinates.hpp"           // NOLINT
#include "locations_reader.hpp"      // NOLINT
#include "locations_grid_utils.hpp"  // NOLINT

int main(int argc, char *argv[]) {
    cv::CommandLineParser cmd(
        argc, argv,
        "{ f loc_file         |           | File with cities }"
        "{ h help             |           | Print help message }");

    std::string loc_file = cmd.get<std::string>("loc_file");
    if (loc_file.empty()) {
        cmd.printMessage();
        return -3;
    }

    try {
        // [1] Read locations from csv file
        Ptr<LocationsReader> reader = LocationsReader::create("csv", loc_file);
        std::vector<std::pair<LocationInfo, Coordinates>> locations =
            reader->read();

        // [2] Compute optimal cell size
        double cell_size = compute_optimal_cell_size(locations);

        // [3] Compute grid partitioning
        std::vector<LocationsSegmentInfo> segments;
        int cols, rows;
        std::tie(segments, cols, rows) = compute_grid(locations, cell_size);

        // Compute median
        std::vector<int> sorted_counts(segments.size());
        for (size_t i = 0; i < segments.size(); ++i) {
            sorted_counts[i] = static_cast<int>(segments[i].locations.size());
        }
        std::sort(sorted_counts.begin(), sorted_counts.end());
        double median = 0.0;
        auto it_nonzero =
            std::find_if(sorted_counts.begin(), sorted_counts.end(),
                         [](int v) { return v > 0; });
        if (it_nonzero != sorted_counts.end()) {
            size_t start = std::distance(sorted_counts.begin(), it_nonzero);
            size_t m = sorted_counts.size() - start;
            median = (m == 0 ? 0.0 : sorted_counts[start + (m - 1) / 2]);
        }

        // [4] Print grid info
        std::cout << "Grid Partitioning Results:" << std::endl;
        std::cout << "\tMedian cities per segment: " << median << std::endl;
        std::cout << "\tMax cities in a segment: " << sorted_counts.back()
                  << std::endl;
        std::cout << "\tGrid dimensions: " << rows << " rows x " << cols
                  << " cols" << std::endl;
        std::cout << "\tTotal segments: " << segments.size() << std::endl;
        std::cout << "\tCell size: " << cell_size << std::endl;

        // [5] Save segments to csv
        std::ofstream seg_out(loc_file.substr(0, loc_file.find_last_of('.')) +
                              "_segments.csv");
        seg_out << "\"segment_id\";\"lat_min\";\"lat_max\";\"lon_min\";\"lon_"
                   "max\";\"neighbors\";\"locations\"\n";
        for (const auto &seg : segments) {
            seg_out << seg.segment_id << ";" << seg.lat_range.first << ";"
                    << seg.lat_range.second << ";" << seg.lon_range.first << ";"
                    << seg.lon_range.second << ";";
            for (size_t i = 0; i < seg.neighbors.size(); ++i) {
                seg_out << seg.neighbors[i];
                if (i + 1 < seg.neighbors.size()) seg_out << ",";
            }
            seg_out << ";";
            for (size_t i = 0; i < seg.locations.size(); ++i) {
                seg_out << seg.locations[i];
                if (i + 1 < seg.locations.size()) seg_out << ",";
            }
            seg_out << "\n";
        }
    } catch (const std::exception &e) {
        std::cout << e.what() << std::endl;
        return -1;
    }
    return 0;
}
