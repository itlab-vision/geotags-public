// Copyright 2025 itlab-vision

#include <string>
#include <vector>
#include <algorithm>

#include "distance.hpp"          // NOLINT
#include "coordinates.hpp"       // NOLINT
#include "nearest_location.hpp"  // NOLINT

void NearestLocation::convert_location_info(
    LocationInfo& loc, const LocationsData::LocationInfo* info) {
    // Convert alt names
    std::vector<std::string> alt_names;
    auto fb_alt_names = info->alt_names();
    alt_names.reserve(fb_alt_names->size());
    for (auto fb_str : *fb_alt_names) {
        alt_names.emplace_back(fb_str->str());
    }

    loc = LocationInfo(info->location_id(), info->country()->str(),
                       info->city()->str(), info->region_id(),
                       info->district_id(), alt_names);
}
