// Copyright 2025 itlab-vision
#pragma once

#include <string>
#include <vector>
#include <optional>

#include "auxiliary.hpp"               // NOLINT
#include "coordinates.hpp"             // NOLINT
#include "location_info.hpp"           // NOLINT
#include "district_info.hpp"           // NOLINT
#include "district_neighbor_info.hpp"  // NOLINT

#include "districts_reader_types.hpp"           // NOLINT
#include "district_neighbors_reader_types.hpp"  // NOLINT

extern "C" {
#include "tg.h"  // NOLINT
}

class DistrictFinder {
 public:
    virtual PrecisionState find_district(
        const DistrictsWrapperType& districts,
        const std::optional<DistrictNeighborsWrapperType>& neighbors,
        const Coordinates& location, LocationInfo& nearest_location,
        DistrictInfo& district) = 0;
    static Ptr<DistrictFinder> create(const std::string& search_type);
};
