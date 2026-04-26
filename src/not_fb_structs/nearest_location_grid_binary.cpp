// Copyright 2025 itlab-vision

#include <string>
#include <vector>
#include <algorithm>
#include <limits>

#include "distance.hpp"          // NOLINT
#include "coordinates.hpp"       // NOLINT
#include "nearest_location.hpp"  // NOLINT

void NearestLocationGridBinary::searchSegment(
    const LocationsSegmentInfo& segment,
    const std::vector<Pair<LocationInfo, Coordinates>>& locations,
    const Ptr<Distance>& dist_calculator, const Coordinates& loc_target,
    LocationInfo& nearest_location, double& min_distance, int& checked) {
    const auto& ids = segment.locations;
    if (ids.empty()) return;

    const int size = ids.size();
    const double target_lat = loc_target.latitude;

    auto process_index = [&](int& idx, int step) {
        if (idx < 0 || idx >= size) return false;

        checked++;

        unsigned int loc_id = ids[idx];
        const auto& loc = locations[loc_id];

        double lat_diff = std::abs(loc.second.latitude - target_lat);
        if (lat_diff > min_distance) return false;

        double d = dist_calculator->calculate(loc.second, loc_target);
        if (d < min_distance) {
            min_distance = d;
            nearest_location = loc.first;
        }

        idx += step;
        return true;
    };

    // Init borders (binary search by latitude)
    auto comp = [&](unsigned int id, double lat) {
        return locations[id].second.latitude < lat;
    };
    auto it = std::lower_bound(ids.begin(), ids.end(), target_lat, comp);

    int right = std::distance(ids.begin(), it);
    int left = right - 1;

    // Expand left/right while within min_distance
    while (left >= 0 || right < size) {
        bool updated = false;

        updated |= process_index(left, -1);
        updated |= process_index(right, 1);

        if (!updated) break;
    }
}
