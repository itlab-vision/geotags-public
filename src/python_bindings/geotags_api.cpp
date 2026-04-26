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
#include "coordinates.hpp"                // NOLINT
#include "distance.hpp"                   // NOLINT
#include "exif_wrapper.hpp"               // NOLINT
#include "locations_reader.hpp"           // NOLINT
#include "district_info.hpp"              // NOLINT
#include "locations_segments_reader.hpp"  // NOLINT
#include "nearest_location.hpp"           // NOLINT
#include "geotags_api.hpp"                // NOLINT

#ifdef USE_FLATBUFFERS_STRUCTURES
#include "locations_generated.hpp"           // NOLINT
#include "locations_segments_generated.hpp"  // NOLINT
#endif

Result get_location(const std::string& image_path,
                    const std::string& reader_type, const std::string& loc_file,
                    const std::string& dist_formula,
                    const std::string& search_type) {
    Result result{};

    try {
        // [1] Get exif data from image
        EXIFData exif_data(image_path);

        // [2] Read locations from csv file
#ifndef USE_FLATBUFFERS_STRUCTURES
        Ptr<LocationsReader> reader =
            LocationsReader::create(reader_type, loc_file);
        std::vector<std::pair<LocationInfo, Coordinates>> locations =
            reader->read();
#else
        CSVLocationsReaderFB reader = CSVLocationsReaderFB(loc_file);
        auto locations = reader.read();
#endif

        // [3] Calculate nearest location
        Ptr<Distance> dist_calculator = Distance::create(dist_formula);
        Coordinates location(exif_data.get_lat(), exif_data.get_lon());
        LocationInfo nearest_location;
        double nearest_distance;

        // [3.1] Read segments if needed
#ifndef USE_FLATBUFFERS_STRUCTURES
        std::vector<LocationsSegmentInfo> segments;
        if (search_type == "grid" || search_type == "grid_binary") {
            std::string grid_file =
                loc_file.substr(0, loc_file.find_last_of('.')) +
                "_segments.csv";
            LocationsSegmentsReader grid_reader =
                LocationsSegmentsReader(grid_file);
            segments = grid_reader.read();
        }
#else
        const flatbuffers::Vector<
            flatbuffers::Offset<LocationsSegmentsData::LocationsSegmentInfo>>*
            segments = nullptr;
        if (search_type == "grid" || search_type == "grid_binary") {
            std::string grid_file =
                loc_file.substr(0, loc_file.find_last_of('.')) +
                "_segments.csv";
            LocationsSegmentsReader grid_reader =
                LocationsSegmentsReader(grid_file);
            segments = grid_reader.read();
        }
#endif

        // [3.2] Search for nearest location
        Ptr<NearestLocation> nearest_location_finder =
            NearestLocation::create(search_type);
        int nearest_loc_state =
            static_cast<int>(nearest_location_finder->calculate(
                locations, segments, dist_calculator, location,
                nearest_location, nearest_distance));

        // [4] Fill result
        result.latitude = location.latitude;
        result.longitude = location.longitude;
        result.distance = nearest_distance;

        std::ostringstream oss;
        oss << nearest_location;
        result.nearest_location = oss.str();

        return result;
    } catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
        return result;
    }
}
