// Copyright 2025 itlab-vision
#pragma once

#include <optional>

#include "district_finder.hpp"  // NOLINT

class DistrictFinderLinear : public DistrictFinder {
 public:
    PrecisionState find_district(
        const DistrictsWrapperType& districts,
        const std::optional<DistrictNeighborsWrapperType>& neighbors,
        const Coordinates& location, LocationInfo& nearest_location,
        DistrictInfo& district);
};
