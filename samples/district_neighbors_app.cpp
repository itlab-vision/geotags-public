// Copyright 2025 itlab-vision
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
#include "coordinates.hpp"               // NOLINT
#include "distance.hpp"                  // NOLINT
#include "exif_wrapper.hpp"              // NOLINT
#include "district_reader.hpp"           // NOLINT
#include "district_info.hpp"             // NOLINT
#include "find_district.hpp"             // NOLINT
#include "find_district_neighbors.hpp"   // NOLINT
#include "print_district_neighbors.hpp"  // NOLINT

#ifdef USE_FLATBUFFERS_STRUCTURES
#include "district_neighbors_reader.hpp"     // NOLINT
#include "district_neighbors_generated.hpp"  // NOLINT
#endif

int main(int argc, char* argv[]) {
    cv::CommandLineParser cmd(
        argc, argv,
        "{ d distr_file         |           | File with districts }"
        "{ h help             |           | Print help message }");
    std::string distr_file = cmd.get<std::string>("distr_file");
    std::string output_file =
        distr_file.substr(0, distr_file.size() - 4) + "_neighbors.csv";
    if (distr_file == "") {
        cmd.printMessage();
        return -3;
    }

    try {
        typedef std::chrono::steady_clock timer;
#ifndef USE_FLATBUFFERS_STRUCTURES
        std::cout << "=== Processing without FlatBuffers ===" << std::endl;
        CSVDistrictsReader reader(distr_file);
        // [1] Read districts from file (.csv)
        auto tstart = timer::now();
        std::vector<std::pair<DistrictInfo, struct tg_geom*>> districts =
            reader.read();
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
            out << "dist_id,neighbors_id" << std::endl;
            for (auto neighbors_vect : neighbors_map) {
                out << neighbors_vect.first << ",";

                if (neighbors_vect.second.empty()) {
                    out << "";
                } else {
                    for (size_t j = 0; j < neighbors_vect.second.size(); ++j) {
                        if (j > 0) out << ",";
                        out << neighbors_vect.second[j];
                    }
                }
                out << std::endl;
            }
            std::cout << "Results saved to: " << output_file << std::endl;
        }

        // Free allocated space
        for (auto district : districts) {
            tg_geom_free(district.second);
        }

        // [4] Test find neighbors
        tstart = timer::now();
        auto kirov = neighbors_map[28];
        tfinish = timer::now();
        std::chrono::duration<double> tsearching_district{tfinish - tstart};

#else
        std::cout << "=== Processing with FlatBuffers ===" << std::endl;

        DistrictNeighborsReader reader(distr_file);
        // [1] Read districts from file (.fb)
        auto tstart = timer::now();
        auto neighbors_map = reader.read();
        auto tfinish = timer::now();
        std::chrono::duration<double> treading_dists{tfinish - tstart};

        // [2] Test find neighbors
        tstart = timer::now();
        auto kirov = neighbors_map->LookupByKey(28);
        if (!kirov) {
            throw std::out_of_range("Key not found");
        }
        tfinish = timer::now();
        std::chrono::duration<double> tsearching_district{tfinish - tstart};
        std::cout << "Kirov Oblast id: " << kirov->id() << std::endl;
#endif
        std::cout << "------------------------------------" << std::endl;
        std::cout << "Time of reading/processing districts from base: "
                  << treading_dists.count() << " s" << std::endl;
#ifndef USE_FLATBUFFERS_STRUCTURES
        std::cout << "Time of searching for districts neighbors: "
                  << tsearching_neighbors.count() << " s" << std::endl;
#endif
        std::cout << "Time of accessing Kirov district data:: "
                  << tsearching_district.count() << " s" << std::endl;
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
