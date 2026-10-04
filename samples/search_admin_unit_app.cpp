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

#include "create_districts_reader.hpp"           // NOLINT
#include "create_district_neighbors_reader.hpp"  // NOLINT

int main(int argc, char* argv[]) {
    cv::CommandLineParser cmd(
        argc, argv,
        "{ i input            |           | Input image }"
#ifndef USE_FLATBUFFERS_STRUCTURES
        "{ t reader_type      |           | Type of data reader (csv, csv_fb) }"
#else
        "{ t reader_type      |           | Type of data reader (csv_fb) }"
#endif
        "{ c city_dist_file   |           | File with city districts }"
        "{ r reg_file         |           | File with regions }"
        "{ h help             |           | Print help message }");

    std::string image_path = cmd.get<std::string>("input");
    std::string reader_type = cmd.get<std::string>("reader_type");
    std::string reg_file = cmd.get<std::string>("reg_file");
    std::string city_dist_file = cmd.get<std::string>("city_dist_file");
    if (image_path == "" || reader_type == "") {
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

        auto city_dist_reader =
            create_districts_reader(reader_type, city_dist_file);
        auto reg_reader = create_districts_reader(reader_type, reg_file);

        // [2] Read cities
        tstart = timer::now();
        auto cities =
            std::visit([](auto&& r) { return r->read(); }, city_dist_reader);
        tfinish = timer::now();
        std::chrono::duration<double> treading_city_districts{tfinish - tstart};

        // [3] Read regions
        tstart = timer::now();
        auto regions =
            std::visit([](auto&& r) { return r->read(); }, reg_reader);
        tfinish = timer::now();
        std::chrono::duration<double> treading_regs{tfinish - tstart};

        Coordinates location(exif_info.get_lat(), exif_info.get_lon());
        LocationInfo nearest_location;
        DistrictInfo city;
        DistrictInfo region;
        std::optional<DistrictNeighborsWrapperType> districts_neighbors;

        // [4] Create linear searcher
        Ptr<DistrictFinder> district_finder = DistrictFinder::create("linear");

        // [5] Search nearest city
        tstart = timer::now();
        PrecisionState city_dist_found = district_finder->find_district(
            cities, districts_neighbors, location, nearest_location, city);
        tfinish = timer::now();
        std::chrono::duration<double> tsearching_nearest_city{tfinish - tstart};

        std::chrono::duration<double> tsearching_nearest_reg{};

        // [6] If not in cities then search nearest region
        if (city_dist_found == PrecisionState::Approximate) {
            tstart = timer::now();
            district_finder->find_district(regions, districts_neighbors,
                                           location, nearest_location, region);
            tfinish = timer::now();
            tsearching_nearest_reg = tfinish - tstart;
        }

        // [7] Print collected data
        std::cout << "------------------------------------" << std::endl;
        std::cout << "Latitude: " << location.latitude << std::endl;
        std::cout << "Longitude: " << location.longitude << std::endl;

        if (city_dist_found == PrecisionState::Exact) {
            (std::cout << "City district:\n" << city);
        } else {
            (std::cout << "Region:\n" << region);
        }
        std::cout << "------------------------------------" << std::endl;
        std::cout << "Time of parsing exif info: " << texif_parsing.count()
                  << " s" << std::endl;
        std::cout << "Time of reading regions from base: "
                  << treading_regs.count() << " s" << std::endl;
        std::cout << "Time of reading city districts from base: "
                  << treading_city_districts.count() << " s" << std::endl;
        std::cout << "Time of searching for nearest city district: "
                  << tsearching_nearest_city.count() << " s" << std::endl;
        std::cout << "Time of searching for nearest region: "
                  << tsearching_nearest_reg.count() << " s" << std::endl;
        std::cout << "------------------------------------" << std::endl;
    } catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
        return -1;
    }
    return 0;
}
