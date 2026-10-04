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
#include <opencv2/core/utils/filesystem.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>

#include "TinyEXIF.h"
#include "auxiliary.hpp"    // NOLINT
#include "coordinates.hpp"  // NOLINT
#include "distance.hpp"     // NOLINT
#include "exif_info.hpp"    // NOLINT
#include "geotags_api.hpp"  // NOLINT

#include "create_locations_reader.hpp"    // NOLINT
#include "create_attractions_reader.hpp"  // NOLINT
#include "create_segments_reader.hpp"     // NOLINT
#include "generic_nearest_point.hpp"      // NOLINT

ResultLocation get_location(const std::string& image_path,
                            const std::string& reader_type,
                            const std::string& loc_file,
                            const std::string& dist_formula,
                            const std::string& search_type) {
    ResultLocation result{};

    try {
        // [1] Get exif data from image
        EXIFInfo exif_info(image_path);

        // [2] Read locations from csv file
        auto locations_reader = create_locations_reader(reader_type, loc_file);
        auto locations =
            std::visit([](auto&& r) { return r->read(); }, locations_reader);

        // [3] Calculate nearest location
        Ptr<Distance> dist_calculator = Distance::create(dist_formula);
        Coordinates location(exif_info.get_lat(), exif_info.get_lon());
        LocationInfo nearest_location;
        double nearest_distance;

        // [3.1] Read segments if needed
        std::optional<SegmentsWrapperType> segments;
        if (search_type == "grid" || search_type == "grid_binary") {
            std::string grid_file =
                loc_file.substr(0, loc_file.find_last_of('.')) +
                "_segments.csv";
            auto locations_segments_reader =
                create_segments_reader(reader_type, grid_file);
            segments = std::visit([](auto&& r) { return r->read(); },
                                  locations_segments_reader);
        }

        // [3.2] Search for nearest location
        auto finder =
            GenericNearestPoint<LocationInfo, LocationsWrapperType>::create(
                search_type);
        finder->calculate(locations, segments, dist_calculator, location,
                          nearest_location, nearest_distance);

        // [4] Fill result
        result.latitude = location.latitude;
        result.longitude = location.longitude;
        result.distance = nearest_distance;
        result.nearest_location = nearest_location;
    } catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
    }

    return result;
}

ResultAttraction get_attraction(const std::string& image_path,
                                const std::string& reader_type,
                                const std::string& loc_file,
                                const std::string& dist_formula,
                                const std::string& search_type) {
    ResultAttraction result{};

    try {
        // [1] Get exif data from image
        EXIFInfo exif_info(image_path);

        // [2] Read locations from csv file
        auto locations_reader = create_locations_reader(reader_type, loc_file);
        auto locations =
            std::visit([](auto&& r) { return r->read(); }, locations_reader);

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
        auto result_f1 = nearest_location_finder->calculate(
            locations, segments, dist_calculator, location, nearest_location,
            nearest_distance);

        if (result_f1 == PrecisionState::Approximate ||
            nearest_location.attraction_db_path.empty()) {
            throw std::runtime_error(
                "No suitable database of attractions found");
        }

        // [4] Read attractions data
        std::string attr_file =
            cv::utils::fs::join(cv::utils::fs::getParent(loc_file),
                                nearest_location.attraction_db_path);

        auto attractions_reader =
            create_attractions_reader(reader_type, attr_file);
        auto attractions =
            std::visit([](auto&& r) { return r->read(); }, attractions_reader);

        // [5] Calculate nearest attraction
        Coordinates attraction(exif_info.get_lat(), exif_info.get_lon());
        AttractionInfo nearest_attraction;

        auto nearest_attraction_finder =
            GenericNearestPoint<AttractionInfo, AttractionsWrapperType>::create(
                search_type);
        PrecisionState precision_result = nearest_attraction_finder->calculate(
            attractions, segments, dist_calculator, location,
            nearest_attraction, nearest_distance);

        // [6] Fill result
        result.latitude = location.latitude;
        result.longitude = location.longitude;
        result.distance = nearest_distance;
        result.nearest_attraction = nearest_attraction;
    } catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
    }

    return result;
}
