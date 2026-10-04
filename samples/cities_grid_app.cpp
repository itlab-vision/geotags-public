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

#include "locations_grid_utils.hpp"     // NOLINT
#include "create_locations_reader.hpp"  // NOLINT

int main(int argc, char* argv[]) {
    cv::CommandLineParser cmd(
        argc, argv,
#ifndef USE_FLATBUFFERS_STRUCTURES
        "{ t reader_type      |           | Type of data reader (csv, csv_fb) }"
#else
        "{ t reader_type      |           | Type of data reader (csv_fb) }"
#endif
        "{ f loc_file         |           | File with cities }"
        "{ h help             |           | Print help message }");

    std::string reader_type = cmd.get<std::string>("reader_type");
    std::string loc_file = cmd.get<std::string>("loc_file");
    if (reader_type == "" || loc_file == "") {
        cmd.printMessage();
        return -3;
    }

    try {
        // [1] Read locations from csv file
        auto locations_reader = create_locations_reader(reader_type, loc_file);
        auto locations =
            std::visit([](auto&& r) { return r->read(); }, locations_reader);

        // [2] Compute optimal cell size
        double cell_size = compute_optimal_cell_size(locations);

        // [3] Compute grid partitioning
        std::vector<SegmentInfo> segments;
        int cols, rows;
        std::tie(segments, cols, rows) = compute_grid(locations, cell_size);

        // Compute median
        std::vector<size_t> sorted_counts(segments.size());
        for (size_t i = 0; i < segments.size(); ++i) {
            sorted_counts[i] = segments[i].points.size();
        }
        std::sort(sorted_counts.begin(), sorted_counts.end());
        double median = 0.0;
        auto it_nonzero =
            std::find_if(sorted_counts.begin(), sorted_counts.end(),
                         [](size_t v) { return v > 0; });
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
        seg_out << segments.size() << "\n";
        seg_out << "\"segment_id\";\"lat_min\";\"lat_max\";\"lon_min\";\"lon_"
                   "max\";\"neighbors\";\"locations\"\n";
        for (const auto& seg : segments) {
            seg_out << seg.segment_id << ";" << seg.lat_range.first << ";"
                    << seg.lat_range.second << ";" << seg.lon_range.first << ";"
                    << seg.lon_range.second << ";";
            for (size_t i = 0; i < seg.neighbors.size(); ++i) {
                seg_out << seg.neighbors[i];
                if (i + 1 < seg.neighbors.size()) seg_out << ",";
            }
            seg_out << ";";
            for (size_t i = 0; i < seg.points.size(); ++i) {
                seg_out << seg.points[i];
                if (i + 1 < seg.points.size()) seg_out << ",";
            }
            seg_out << "\n";
        }
    } catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
        return -1;
    }
    return 0;
}
