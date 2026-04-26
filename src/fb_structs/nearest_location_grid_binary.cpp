// Copyright 2025 itlab-vision

#include <limits>
#include <string>
#include <vector>
#include <algorithm>
#include <unordered_set>

#include "distance.hpp"          // NOLINT
#include "coordinates.hpp"       // NOLINT
#include "nearest_location.hpp"  // NOLINT

void NearestLocationGridBinary::searchSegment(
    const LocationsSegmentsData::LocationsSegmentInfo* segment,
    const flatbuffers::Vector<flatbuffers::Offset<LocationsData::Location>>*
        locations,
    const Ptr<Distance>& dist_calculator, const Coordinates& loc_target,
    int& nearest_location_id, double& min_distance, int& checked) {
    auto loc_ids_fb = segment->locations();
    if (!loc_ids_fb || loc_ids_fb->size() == 0) return;

    const int size = loc_ids_fb->size();
    const double target_lat = loc_target.latitude;

    auto process_index = [&](int& idx, int step) {
        if (idx < 0 || idx >= size) return false;

        checked++;

        auto loc_id = loc_ids_fb->Get(idx);
        auto loc = locations->Get(loc_id);

        double lat_diff = std::abs(loc->coords()->latitude() - target_lat);
        if (lat_diff > min_distance) return false;

        double d = dist_calculator->calculate(loc->coords(), loc_target);
        if (d < min_distance) {
            min_distance = d;
            nearest_location_id = loc_id;
        }

        idx += step;
        return true;
    };

    // Init borders (binary search by latitude)
    auto comp = [&](uint32_t loc_id, double lat) {
        return locations->Get(loc_id)->coords()->latitude() < lat;
    };
    auto it = std::lower_bound(loc_ids_fb->begin(), loc_ids_fb->end(),
                               target_lat, comp);

    int right = std::distance(loc_ids_fb->begin(), it);
    int left = right - 1;

    // Expand left/right while within min_distance
    while (left >= 0 || right < size) {
        bool updated = false;

        updated |= process_index(left, -1);
        updated |= process_index(right, 1);

        if (!updated) break;
    }
}
