// Copyright 2025 itlab-vision
#pragma once

#include <string>

struct Result {
    double latitude;
    double longitude;
    std::string nearest_location;
    double distance;
};

Result get_location(const std::string& image_path,
                    const std::string& reader_type, const std::string& loc_file,
                    const std::string& dist_formula,
                    const std::string& search_type);
