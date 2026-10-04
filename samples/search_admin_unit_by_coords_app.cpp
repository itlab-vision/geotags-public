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

#include "coordinates.hpp"            // NOLINT
#include "distance.hpp"               // NOLINT
#include "exif_info.hpp"              // NOLINT
#include "generic_nearest_point.hpp"  // NOLINT
#include "district_finder.hpp"        // NOLINT

#include "create_locations_reader.hpp"           // NOLINT
#include "create_segments_reader.hpp"            // NOLINT
#include "create_districts_reader.hpp"           // NOLINT
#include "create_district_neighbors_reader.hpp"  // NOLINT

int main(int argc, char* argv[]) {
    cv::CommandLineParser cmd(
        argc, argv,
        "{ lat   | | Latitude in degrees (-90..90) }"
        "{ lon   | | Longitude in degrees (-180..180) }"
        "{ df    | | District file }"
        "{ dnf   | | District neighbors file }"
#ifndef USE_FLATBUFFERS_STRUCTURES
        "{ t reader_type      |           | Type of data reader (csv, csv_fb) }"
#else
        "{ t reader_type      |           | Type of data reader (csv_fb) }"
#endif
        "{ st    | | Search type (linear, neighbors) }"
        "{ lf    | | Locations file }"
        "{ help  | | Print help message }");

    double lat = cmd.get<double>("lat");
    double lon = cmd.get<double>("lon");
    std::string reader_type = cmd.get<std::string>("reader_type");
    std::string distr_file = cmd.get<std::string>("df");
    std::string distr_nb_file = cmd.get<std::string>("dnf");
    std::string search_type = cmd.get<std::string>("st");
    std::string loc_file = cmd.get<std::string>("lf");
    if (!cmd.has("lat") || !cmd.has("lon") || reader_type.empty() ||
        distr_file.empty() || search_type.empty() || loc_file.empty()) {
        cmd.printMessage();
        return -3;
    }
    if (search_type == "neighbors" && distr_nb_file.empty()) {
        cmd.printMessage();
        return -3;
    }

    // Additional validation of coordinate ranges
    if (lat < -90.0 || lat > 90.0) {
        std::cerr << "Error: Latitude must be between -90 and 90 degrees."
                  << std::endl;
        return -2;
    }
    if (lon < -180.0 || lon > 180.0) {
        std::cerr << "Error: Longitude must be between -180 and 180 degrees."
                  << std::endl;
        return -2;
    }

    try {
        typedef std::chrono::steady_clock timer;

        // [1] Setup locations
        // [1.1] Read locations
        auto locations_reader = create_locations_reader(reader_type, loc_file);
        auto locations =
            std::visit([](auto&& r) { return r->read(); }, locations_reader);

        // [1.2] Calculate nearest location
        Ptr<Distance> dist_calculator = Distance::create("haversine");
        std::optional<SegmentsWrapperType> segments;
        Coordinates location(lat, lon);
        LocationInfo nearest_location;
        double nearest_distance;

        auto nearest_location_finder =
            GenericNearestPoint<LocationInfo, LocationsWrapperType>::create(
                "linear");
        PrecisionState precision_result = nearest_location_finder->calculate(
            locations, segments, dist_calculator, location, nearest_location,
            nearest_distance);

        // [2] Setup districts
        // [2.1] Read districts
        auto districts_reader =
            create_districts_reader(reader_type, distr_file);
        auto tstart = timer::now();
        auto districts =
            std::visit([](auto&& r) { return r->read(); }, districts_reader);
        auto tfinish = timer::now();
        std::chrono::duration<double> treading_dists{tfinish - tstart};

        // [2.2] Read districts neighbors if needed
        std::optional<DistrictNeighborsWrapperType> districts_neighbors;
        if (search_type == "neighbors") {
            auto neighbor_reader =
                create_district_neighbors_reader(reader_type, distr_nb_file);
            tstart = timer::now();
            districts_neighbors =
                std::visit([](auto&& r) { return r->read(); }, neighbor_reader);
        }
        tfinish = timer::now();
        std::chrono::duration<double> treading_dist_neighbors{tfinish - tstart};

        // [2.3] Create district searcher
        Ptr<DistrictFinder> district_finder =
            DistrictFinder::create(search_type);

        // [3] Search district
        DistrictInfo district;
        tstart = timer::now();
        PrecisionState dist_result = district_finder->find_district(
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
        std::cout << "Time of reading districts from base: "
                  << treading_dists.count() << " s" << std::endl;
        if (search_type == "neighbors") {
            std::cout << "Time of reading district neighbors from base: "
                      << treading_dist_neighbors.count() << " s" << std::endl;
        }
        std::cout << "Time of searching for district: "
                  << tsearching_district.count() << " s" << std::endl;
        std::cout << "------------------------------------" << std::endl;
    } catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
        return -1;
    }
    return 0;
}
