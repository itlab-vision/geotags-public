// Copyright 2025 itlab-vision
#include <algorithm>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>

#include "TinyEXIF.h"
#include "exif_info.hpp"                 // NOLINT
#include "district_neighbors_utils.hpp"  // NOLINT
#include "create_districts_reader.hpp"   // NOLINT

int main(int argc, char* argv[]) {
    cv::CommandLineParser cmd(
        argc, argv,
#ifndef USE_FLATBUFFERS_STRUCTURES
        "{ t reader_type      |           | Type of data reader (csv, csv_fb) }"
#else
        "{ t reader_type      |           | Type of data reader (csv_fb) }"
#endif
        "{ d distr_file       |           | File with districts }"
        "{ h help             |           | Print help message }");
    std::string reader_type = cmd.get<std::string>("reader_type");
    std::string distr_file = cmd.get<std::string>("distr_file");
    std::string output_file =
        distr_file.substr(0, distr_file.size() - 4) + "_neighbors.csv";
    if (reader_type == "" || distr_file == "") {
        cmd.printMessage();
        return -3;
    }

    try {
        typedef std::chrono::steady_clock timer;
        std::cout << "=== Processing ===" << std::endl;

        // [1] Read districts from file (.csv)
        auto districts_reader =
            create_districts_reader(reader_type, distr_file);
        auto tstart = timer::now();
        auto districts =
            std::visit([](auto&& r) { return r->read(); }, districts_reader);
        auto tfinish = timer::now();
        std::chrono::duration<double> treading_dists{tfinish - tstart};

        // [2] Find neighbors for every district
        tstart = timer::now();
        auto neighbors_map = find_district_neighbors(districts);
        tfinish = timer::now();
        std::chrono::duration<double> tsearching_neighbors{tfinish - tstart};

        // [3] Save results of searching
        if (!output_file.empty()) {
            std::ofstream out(output_file);
            out << neighbors_map.size() << std::endl;
            out << "dist_id;neighbors_id" << std::endl;
            for (const auto& [dist_id, neighbors_vect] : neighbors_map) {
                out << dist_id << ";";
                if (neighbors_vect.empty()) {
                    out << "";
                } else {
                    for (size_t j = 0; j < neighbors_vect.size(); ++j) {
                        if (j > 0) out << ",";
                        out << neighbors_vect[j];
                    }
                }
                out << std::endl;
            }
            std::cout << "Results saved to: " << output_file << std::endl;
        }

        // [4] Print collected data
        std::cout << "------------------------------------" << std::endl;
        std::cout << "Time of reading/processing districts from base: "
                  << treading_dists.count() << " s" << std::endl;
        std::cout << "Time of searching for districts neighbors: "
                  << tsearching_neighbors.count() << " s" << std::endl;
        std::cout << "------------------------------------" << std::endl;

        std::cout << "Neighbors:" << std::endl;
        print_district_neighbors(neighbors_map);
        std::cout << "------------------------------------" << std::endl;
    } catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
        return -1;
    }
    return 0;
}
