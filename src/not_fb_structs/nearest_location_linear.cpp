// Copyright 2025 itlab-vision

#include <string>
#include <vector>
#include <algorithm>

#include "distance.hpp"          // NOLINT
#include "coordinates.hpp"       // NOLINT
#include "nearest_location.hpp"  // NOLINT

NearestLocationState NearestLocationLinear::calculate(
    const std::vector<Pair<LocationInfo, Coordinates>>& locations,
    const std::vector<LocationsSegmentInfo>& segments,
    const Ptr<Distance>& dist_calculator, const Coordinates& location,
    LocationInfo& nearest_location, double& min_distance,
    const double threshold) {
    if (locations.size() == 0) {
        throw std::runtime_error("List of available locations is empty");
    }

    nearest_location = locations[0].first;
    min_distance = dist_calculator->calculate(locations[0].second, location);

    for (int i = 1; i < locations.size(); i++) {
        double distance =
            dist_calculator->calculate(locations[i].second, location);
        if (distance < min_distance) {
            nearest_location = locations[i].first;
            min_distance = distance;
        }
    }

    if (threshold > 0 && min_distance > threshold) {
        return NearestLocationState::ApproximateLocation;
    }
    return NearestLocationState::ExactLocation;
}
