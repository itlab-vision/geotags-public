// Copyright 2025 itlab-vision

#include <string>
#include <stdexcept>

#include "district_finder.hpp"           // NOLINT
#include "district_finder_linear.hpp"    // NOLINT
#include "district_finder_neighbor.hpp"  // NOLINT

Ptr<DistrictFinder> DistrictFinder::create(const std::string& search_type) {
    if (search_type == "linear") {
        return Ptr<DistrictFinder>(new DistrictFinderLinear());
    } else if (search_type == "neighbors") {
        return Ptr<DistrictFinder>(new DistrictFinderNeighbor());
    }
    throw std::invalid_argument("Unknown search type: " + search_type);
}
