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

#include "TinyEXIF.h"
#include "coordinates.hpp"      // NOLINT
#include "distance.hpp"         // NOLINT
#include "exif_info.hpp"        // NOLINT
#include "district_finder.hpp"  // NOLINT

#include "create_districts_reader.hpp"  // NOLINT

int main(int argc, char* argv[]) {
    cv::CommandLineParser cmd(
        argc, argv,
        "{ i input            |           | Input image }"
#ifndef USE_FLATBUFFERS_STRUCTURES
        "{ t reader_type      |           | Type of data reader (csv, csv_fb) }"
#else
        "{ t reader_type      |           | Type of data reader (csv_fb) }"
#endif
        "{ d distr_file       |           | File with districts }"
        "{ h help             |           | Print help message }");

    std::string image_path = cmd.get<std::string>("input");
    std::string reader_type = cmd.get<std::string>("reader_type");
    std::string distr_file = cmd.get<std::string>("distr_file");
    if (image_path == "" || reader_type == "" || distr_file == "") {
        cmd.printMessage();
        return -3;
    }

    try {
        typedef std::chrono::steady_clock timer;

        // [1] Read photo
        auto tstart = timer::now();
        EXIFInfo exif_info(image_path);
        auto tfinish = timer::now();
        std::chrono::duration<double> texif_parsing{tfinish - tstart};

        // [2] Read districts
        auto districts_reader =
            create_districts_reader(reader_type, distr_file);
        tstart = timer::now();
        auto districts =
            std::visit([](auto&& r) { return r->read(); }, districts_reader);
        tfinish = timer::now();
        std::chrono::duration<double> treading_dists{tfinish - tstart};

        Coordinates location(exif_info.get_lat(), exif_info.get_lon());
        LocationInfo nearest_location;
        DistrictInfo district;
        std::optional<DistrictNeighborsWrapperType> districts_neighbors;

        // [2] Create linear searcher
        Ptr<DistrictFinder> district_finder = DistrictFinder::create("linear");

        // [3] Search nearest city
        tstart = timer::now();
        PrecisionState result = district_finder->find_district(
            districts, districts_neighbors, location, nearest_location,
            district);
        tfinish = timer::now();
        std::chrono::duration<double> tsearching_district{tfinish - tstart};

        // [4] Print collected data
        std::cout << "------------------------------------" << std::endl;
        std::cout << "Latitude: " << location.latitude << std::endl;
        std::cout << "Longitude: " << location.longitude << std::endl;
        std::cout << "District:\n" << district;
        std::cout << "------------------------------------" << std::endl;
        std::cout << "Time of parsing exif info: " << texif_parsing.count()
                  << " s" << std::endl;
        std::cout << "Time of reading districts from base: "
                  << treading_dists.count() << " s" << std::endl;
        std::cout << "Time of searching for district: "
                  << tsearching_district.count() << " s" << std::endl;
        std::cout << "------------------------------------" << std::endl;
    } catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
        return -1;
    }
    return 0;
}
