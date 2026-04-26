// Copyright 2025 itlab-vision
#pragma once

#include <vector>

#include "auxiliary.hpp"      // NOLINT
#include "location_info.hpp"  // NOLINT
#include "district_info.hpp"  // NOLINT
#include "coordinates.hpp"    // NOLINT

extern "C" {
#include "tg.h"  // NOLINT
}

NearestLocationState find_district(
    const std::vector<Pair<DistrictInfo, tg_geom *>> &districts,
    const Coordinates &location, DistrictInfo &district);
