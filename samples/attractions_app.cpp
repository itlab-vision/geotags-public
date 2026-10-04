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
#include <opencv2/core/utils/filesystem.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>

#include "TinyEXIF.h"
#include "auxiliary.hpp"    // NOLINT
#include "coordinates.hpp"  // NOLINT
#include "distance.hpp"     // NOLINT
#include "exif_info.hpp"    // NOLINT

#include "create_locations_reader.hpp"    // NOLINT
#include "create_attractions_reader.hpp"  // NOLINT
#include "create_segments_reader.hpp"     // NOLINT
#include "generic_nearest_point.hpp"      // NOLINT

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
        "type of search for nearest location and nearest attraction (linear) }"
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

        // Create empty segments vector
        std::optional<SegmentsWrapperType> segments;

        // [3.1] Calculate nearest location
        Ptr<Distance> dist_calculator = Distance::create(dist_formula);
        Coordinates location(exif_info.get_lat(), exif_info.get_lon());
        LocationInfo nearest_location;
        double nearest_distance;

        auto nearest_location_finder =
            GenericNearestPoint<LocationInfo, LocationsWrapperType>::create(
                search_type);
        tstart = timer::now();
        auto result_f1 = nearest_location_finder->calculate(
            locations, segments, dist_calculator, location, nearest_location,
            nearest_distance);
        tfinish = timer::now();
        std::chrono::duration<double> tsearching_nearest_loc{tfinish - tstart};

        if (result_f1 == PrecisionState::Approximate ||
            nearest_location.attraction_db_path.empty()) {
            std::cout << "------------------------------------" << std::endl;
            std::cout << "No corresponding location found." << std::endl;
            std::cout << "------------------------------------" << std::endl;
            std::cout << "Latitude: " << location.latitude << std::endl;
            std::cout << "Longitude: " << location.longitude << std::endl;
            std::cout << "Datetime: " << exif_info.get_datetime() << std::endl;
            std::cout << "Original datetime: "
                      << exif_info.get_datetime_original() << std::endl;
            std::cout << "------------------------------------" << std::endl;
            std::cout << "Time of parsing exif info: " << texif_parsing.count()
                      << " seconds" << std::endl;
            std::cout << "Time of reading locations from base: "
                      << treading_locs.count() << " s" << std::endl;
            std::cout << "Time of searching for nearest location: "
                      << tsearching_nearest_loc.count() << " s" << std::endl;
            std::cout << "------------------------------------" << std::endl;
            return 0;
        }

        // [4] Read attractions data
        std::string attr_file =
            cv::utils::fs::join(cv::utils::fs::getParent(loc_file),
                                nearest_location.attraction_db_path);

        auto attractions_reader =
            create_attractions_reader(reader_type, attr_file);
        tstart = timer::now();
        auto attractions =
            std::visit([](auto&& r) { return r->read(); }, attractions_reader);
        tfinish = timer::now();
        std::chrono::duration<double> treading_attrs{tfinish - tstart};

        // [5] Calculate nearest attraction
        Coordinates attraction(exif_info.get_lat(), exif_info.get_lon());
        AttractionInfo nearest_attraction;

        auto nearest_attraction_finder =
            GenericNearestPoint<AttractionInfo, AttractionsWrapperType>::create(
                search_type);
        tstart = timer::now();
        PrecisionState precision_result = nearest_attraction_finder->calculate(
            attractions, segments, dist_calculator, location,
            nearest_attraction, nearest_distance);
        tfinish = timer::now();
        std::chrono::duration<double> tsearching_nearest_attr{tfinish - tstart};

        // [6] Print collected data
        std::cout << "------------------------------------" << std::endl;
        std::cout << "Latitude: " << attraction.latitude << std::endl;
        std::cout << "Longitude: " << attraction.longitude << std::endl;
        std::cout << "Datetime: " << exif_info.get_datetime() << std::endl;
        std::cout << "Original datetime: " << exif_info.get_datetime_original()
                  << std::endl;
        std::cout << "Nearest attraction:\n" << nearest_attraction;
        std::cout << "Distance: " << nearest_distance << " km" << std::endl;
        std::cout << "Precision: "
                  << (precision_result == PrecisionState::Exact ? "Exact"
                                                                : "Approximate")
                  << std::endl;
        std::cout << "------------------------------------" << std::endl;
        std::cout << "Time of parsing exif info: " << texif_parsing.count()
                  << " seconds" << std::endl;
        std::cout << "Time of reading locations from base: "
                  << treading_locs.count() << " s" << std::endl;
        std::cout << "Time of searching for nearest location: "
                  << tsearching_nearest_loc.count() << " s" << std::endl;
        std::cout << "Time of reading attractions from base: "
                  << treading_attrs.count() << " seconds" << std::endl;
        std::cout << "Time of searching nearest attraction: "
                  << tsearching_nearest_attr.count() << " seconds" << std::endl;
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
    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
        return -1;
    }
    return 0;
}
