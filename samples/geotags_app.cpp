// Copyright 2025 itlab-vision
#include <chrono>

#include <fstream>
#include <iomanip>
#include <iostream>
#include <optional>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>

#include "TinyEXIF.h"
#include "auxiliary.hpp"    // NOLINT
#include "coordinates.hpp"  // NOLINT
#include "distance.hpp"     // NOLINT
#include "exif_info.hpp"    // NOLINT

#include "create_locations_reader.hpp"  // NOLINT
#include "create_segments_reader.hpp"   // NOLINT
#include "generic_nearest_point.hpp"    // NOLINT

int main(int argc, char* argv[]) {
    cv::CommandLineParser cmd(
        argc, argv,
        "{ i input            |           | Input image }"
#ifndef USE_FLATBUFFERS_STRUCTURES
        "{ t reader_type      |           | Type of data reader (csv, csv_fb) }"
#else
        "{ t reader_type      |           | Type of data reader (csv_fb) }"
#endif
        "{ f loc_file         |           | File with locations }"
        "{ s silent           |           | Flag to display image }"
        "{ d dist_formula     |           | Flag to define formula "
        "to compute distance (haversine, haversine_approx) }"
        "{ st search_type     |           | Flag to define "
        "type of search for nearest location (linear, grid, grid_binary) }"
        "{ h help             |           | Print help message }");

    std::string image_path = cmd.get<std::string>("input");
    std::string reader_type = cmd.get<std::string>("reader_type");
    std::string loc_file = cmd.get<std::string>("loc_file");
    std::string dist_formula = cmd.get<std::string>("dist_formula");
    std::string search_type = cmd.get<std::string>("search_type");
    if (image_path == "" || reader_type == "" || loc_file == "" ||
        dist_formula == "" || search_type == "") {
        cmd.printMessage();
        return -3;
    }

    try {
        typedef std::chrono::steady_clock timer;

        // [1] Get exif data from image
        auto tstart = timer::now();
        EXIFInfo exif_info(image_path);
        auto tfinish = timer::now();
        std::chrono::duration<double> texif_parsing{tfinish - tstart};

        // [2] Read locations from csv file
        auto locations_reader = create_locations_reader(reader_type, loc_file);
        tstart = timer::now();
        auto locations =
            std::visit([](auto&& r) { return r->read(); }, locations_reader);
        tfinish = timer::now();
        std::chrono::duration<double> treading_locs{tfinish - tstart};

        // [3] Calculate nearest location
        Ptr<Distance> dist_calculator = Distance::create(dist_formula);
        Coordinates location(exif_info.get_lat(), exif_info.get_lon());
        LocationInfo nearest_location;
        double nearest_distance;

        // [3.1] Read segments if needed
        std::optional<SegmentsWrapperType> segments;
        std::chrono::duration<double> treading_loc_segs{0};
        if (search_type == "grid" || search_type == "grid_binary") {
            std::string grid_file =
                loc_file.substr(0, loc_file.find_last_of('.')) +
                "_segments.csv";
            auto locations_segments_reader =
                create_segments_reader(reader_type, grid_file);
            tstart = timer::now();
            segments = std::visit([](auto&& r) { return r->read(); },
                                  locations_segments_reader);
            tfinish = timer::now();
            treading_loc_segs = std::chrono::duration<double>{tfinish - tstart};
        }

        // [3.2] Search for nearest location
        auto finder =
            GenericNearestPoint<LocationInfo, LocationsWrapperType>::create(
                search_type);
        tstart = timer::now();
        PrecisionState precision_result =
            finder->calculate(locations, segments, dist_calculator, location,
                              nearest_location, nearest_distance);
        tfinish = timer::now();
        std::chrono::duration<double> tsearching_nearest_loc{tfinish - tstart};

        // [4] Print collected data
        std::cout << "------------------------------------" << std::endl;
        std::cout << "Latitude: " << location.latitude << std::endl;
        std::cout << "Longitude: " << location.longitude << std::endl;
        std::cout << "Datetime: " << exif_info.get_datetime() << std::endl;
        std::cout << "Original datetime: " << exif_info.get_datetime_original()
                  << std::endl;
        std::cout << "Nearest location:\n" << nearest_location;
        std::cout << "Distance: " << nearest_distance << " km" << std::endl;
        std::cout << "Precision: "
                  << (precision_result == PrecisionState::Exact ? "Exact"
                                                                : "Approximate")
                  << std::endl;
        std::cout << "------------------------------------" << std::endl;
        std::cout << "Time of parsing exif info: " << texif_parsing.count()
                  << " s" << std::endl;
        std::cout << "Time of reading locations from base: "
                  << treading_locs.count() << " s" << std::endl;
        if (search_type == "grid" || search_type == "grid_binary") {
            std::cout << "Time of reading location segments from base: "
                      << treading_loc_segs.count() << " s" << std::endl;
        }
        std::cout << "Time of searching for nearest location: "
                  << tsearching_nearest_loc.count() << " s" << std::endl;
        std::cout << "------------------------------------" << std::endl;

        // Show image
        if (!cmd.get<bool>("silent")) {
            cv::Mat img = cv::imread(image_path, cv::IMREAD_COLOR);
            if (img.empty()) {
                std::string message = "Could not read the image: ";
                std::cout << message << image_path << std::endl;
                return 1;
            }
            const char* winName = "Display window";
            cv::namedWindow(winName, cv::WINDOW_NORMAL);
            cv::imshow(winName, img);
            cv::waitKey(0);
        }
    } catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
        return -1;
    }
    return 0;
}
