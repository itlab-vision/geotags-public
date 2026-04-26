// Copyright 2025 itlab-vision
#include <string>

#include "distance.hpp"  // NOLINT

Ptr<Distance> Distance::create(const std::string &dist_formula) {
    if (dist_formula == "haversine") {
        return Ptr<Distance>(new HaversineDistance());
    } else if (dist_formula == "haversine_approx") {
        return Ptr<Distance>(new HaversineApproxDistance());
    } else {
        throw std::runtime_error("Unsupported distance type.");
    }
}
