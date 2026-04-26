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
#include "exif_wrapper.hpp"     // NOLINT
#include "district_reader.hpp"  // NOLINT
#include "find_district.hpp"    // NOLINT

int main(int argc, char* argv[]) {
    cv::CommandLineParser cmd(
        argc, argv,
        "{ i input            |           | Input image }"
        "{ t reader_type      |           | Type of data reader (csv, csv_fb) }"
        "{ d distr_file         |           | File with districts }"
        "{ h help             |           | Print help message }");

    std::string image_path = cmd.get<std::string>("input");
    std::string reader_type = cmd.get<std::string>("reader_type");
    std::string distr_file = cmd.get<std::string>("distr_file");
    if (image_path == "" || reader_type == "") {
        cmd.printMessage();
        return -3;
    }

    try {
        typedef std::chrono::steady_clock timer;
        auto tstart = timer::now();
        EXIFData exif_data(image_path);
        auto tfinish = timer::now();
        std::chrono::duration<double> texif_parsing{tfinish - tstart};

        Ptr<DistrictsReader> reader =
            DistrictsReader::create(reader_type, distr_file);
        tstart = timer::now();
        std::vector<std::pair<DistrictInfo, struct tg_geom*>> districts =
            reader->read();
        tfinish = timer::now();
        std::chrono::duration<double> treading_dists{tfinish - tstart};

        Coordinates location(exif_data.get_lat(), exif_data.get_lon());
        DistrictInfo district;
        tstart = timer::now();
        find_district(districts, location, district);
        tfinish = timer::now();
        std::chrono::duration<double> tsearching_nearest_dist{tfinish - tstart};

        for (std::vector<std::pair<DistrictInfo, struct tg_geom*>>::iterator
                 it = districts.begin();
             it != districts.end(); ++it) {
            tg_geom_free(it->second);
        }
        std::cout << "------------------------------------" << std::endl;
        std::cout << "Latitude: " << location.latitude << std::endl;
        std::cout << "Longitude: " << location.longitude << std::endl;
        std::cout << "District:\n" << district;
        std::cout << "------------------------------------" << std::endl;
        std::cout << "Time of parsing exif info: " << texif_parsing.count()
                  << " s" << std::endl;
        std::cout << "Time of reading locations from base: "
                  << treading_dists.count() << " s" << std::endl;
        std::cout << "Time of searching for nearest location: "
                  << tsearching_nearest_dist.count() << " s" << std::endl;
        std::cout << "------------------------------------" << std::endl;
    } catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
        return -1;
    }
    return 0;
}
