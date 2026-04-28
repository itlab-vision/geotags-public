// Copyright 2025 itlab-vision

#include <string>
#include <vector>
#include <algorithm>

#include "distance.hpp"          // NOLINT
#include "coordinates.hpp"       // NOLINT
#include "nearest_location.hpp"  // NOLINT

NearestLocationState NearestLocationLinear::calculate(
    const flatbuffers::Vector<flatbuffers::Offset<LocationsData::Location>>*
        locations,
    const flatbuffers::Vector<
        flatbuffers::Offset<LocationsSegmentsData::LocationsSegmentInfo>>*
        segments,
    const Ptr<Distance>& dist_calculator, const Coordinates& location,
    LocationInfo& nearest_location, double& min_distance,
    const double threshold) {
    if (!locations || locations->size() == 0) {
        throw std::runtime_error("List of available locations is empty");
    }

    auto first_loc = locations->Get(0);
    auto nearest_location_fb = first_loc;
    min_distance = dist_calculator->calculate(first_loc->coords(), location);

    for (auto loc : *locations) {
        double distance = dist_calculator->calculate(loc->coords(), location);
        if (distance < min_distance) {
            nearest_location_fb = loc;
            min_distance = distance;
        }
    }

    convert_location_info(nearest_location, nearest_location_fb->info());

    if (threshold > 0 && min_distance > threshold) {
        return NearestLocationState::ApproximateLocation;
    }
    return NearestLocationState::ExactLocation;
}
