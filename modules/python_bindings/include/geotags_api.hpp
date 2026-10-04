// Copyright 2025 itlab-vision
#pragma once

#include <string>

#include "attraction_info.hpp"  // NOLINT
#include "location_info.hpp"    // NOLINT

struct ResultLocation {
    double latitude;
    double longitude;
    LocationInfo nearest_location;
    double distance;
};

ResultLocation get_location(const std::string& image_path,
                            const std::string& reader_type,
                            const std::string& loc_file,
                            const std::string& dist_formula,
                            const std::string& search_type);

struct ResultAttraction {
    double latitude;
    double longitude;
    AttractionInfo nearest_attraction;
    double distance;
};

ResultAttraction get_attraction(const std::string& image_path,
                                const std::string& reader_type,
                                const std::string& loc_file,
                                const std::string& dist_formula,
                                const std::string& search_type);
