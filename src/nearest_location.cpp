// Copyright 2025 itlab-vision

#include <string>
#include <vector>
#include <algorithm>

#include "distance.hpp"          // NOLINT
#include "coordinates.hpp"       // NOLINT
#include "nearest_location.hpp"  // NOLINT

Ptr<NearestLocation> NearestLocation::create(const std::string& search_type) {
    if (search_type == "linear") {
        return Ptr<NearestLocation>(new NearestLocationLinear());
    } else if (search_type == "grid") {
        return Ptr<NearestLocation>(new NearestLocationGrid());
    } else if (search_type == "grid_binary") {
        return Ptr<NearestLocation>(new NearestLocationGridBinary());
    }
    throw std::invalid_argument("Unknown search type: " + search_type);
}
