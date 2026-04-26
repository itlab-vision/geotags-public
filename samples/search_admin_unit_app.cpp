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
#include "coordinates.hpp"       // NOLINT
#include "distance.hpp"          // NOLINT
#include "exif_wrapper.hpp"      // NOLINT
#include "locations_reader.hpp"  // NOLINT
#include "district_reader.hpp"   // NOLINT
#include "find_district.hpp"     // NOLINT

int main(int argc, char* argv[]) {
    cv::CommandLineParser cmd(
        argc, argv,
        "{ i input            |           | Input image }"
        "{ t reader_type      |           | Type of data reader (csv, csv_fb) }"
        "{ c city_file        |           | File with cities }"
        "{ r reg_file         |           | File with regions }"
        "{ h help             |           | Print help message }");

    std::string image_path = cmd.get<std::string>("input");
    std::string reader_type = cmd.get<std::string>("reader_type");
    std::string reg_file = cmd.get<std::string>("reg_file");
    std::string city_file = cmd.get<std::string>("city_file");
    if (image_path == "" || reader_type == "") {
        cmd.printMessage();
        return -3;
    }

    try {
        typedef std::chrono::steady_clock timer;

        // [1] Read photo
        auto tstart = timer::now();
        EXIFData exif_data(image_path);
        auto tfinish = timer::now();
        std::chrono::duration<double> texif_parsing{tfinish - tstart};

        Ptr<DistrictsReader> city_reader =
            DistrictsReader::create(reader_type, city_file);
        Ptr<DistrictsReader> reg_reader =
            DistrictsReader::create(reader_type, reg_file);
        // [2] Read cities
        tstart = timer::now();
        std::vector<std::pair<DistrictInfo, struct tg_geom*>> cities =
            city_reader->read();
        tfinish = timer::now();
        std::chrono::duration<double> treading_cities{tfinish - tstart};

        // [3] Read regions
        tstart = timer::now();
        std::vector<std::pair<DistrictInfo, struct tg_geom*>> regions =
            reg_reader->read();
        tfinish = timer::now();
        std::chrono::duration<double> treading_regs{tfinish - tstart};

        Coordinates location(exif_data.get_lat(), exif_data.get_lon());
        DistrictInfo city;
        DistrictInfo region;

        // [4] Search nearest city
        tstart = timer::now();
        NearestLocationState city_found = find_district(cities, location, city);
        tfinish = timer::now();
        std::chrono::duration<double> tsearching_nearest_city{tfinish - tstart};

        std::chrono::duration<double> tsearching_nearest_reg{};

        // [5] If not in cities then search nearest region
        if (city_found == NearestLocationState::ApproximateLocation) {
            tstart = timer::now();
            find_district(regions, location, region);
            tfinish = timer::now();
            tsearching_nearest_reg = tfinish - tstart;
        }

        // [6] Clear data
        for (std::vector<std::pair<DistrictInfo, struct tg_geom*>>::iterator
                 it = cities.begin();
             it != cities.end(); ++it) {
            tg_geom_free(it->second);
        }

        for (std::vector<std::pair<DistrictInfo, struct tg_geom*>>::iterator
                 it = regions.begin();
             it != regions.end(); ++it) {
            tg_geom_free(it->second);
        }

        // [7] Print collected data
        std::cout << "------------------------------------" << std::endl;
        std::cout << "Latitude: " << location.latitude << std::endl;
        std::cout << "Longitude: " << location.longitude << std::endl;

        if (city_found == NearestLocationState::ExactLocation) {
            (std::cout << "City:\n" << city);
        } else {
            (std::cout << "Region:\n" << region);
        }
        std::cout << "------------------------------------" << std::endl;
        std::cout << "Time of parsing exif info: " << texif_parsing.count()
                  << " s" << std::endl;
        std::cout << "Time of reading regions from base: "
                  << treading_regs.count() << " s" << std::endl;
        std::cout << "Time of reading cities from base: "
                  << treading_cities.count() << " s" << std::endl;
        std::cout << "Time of searching for nearest city: "
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
