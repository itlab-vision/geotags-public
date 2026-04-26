// Copyright 2025 itlab-vision
#pragma once
#include <chrono>

#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include <unordered_map>

#include "auxiliary.hpp"      // NOLINT
#include "district_info.hpp"  // NOLINT

extern "C" {
#include "tg.h"  // NOLINT
}

std::unordered_map<unsigned int, std::vector<unsigned int>>
find_district_neighbors(
    const std::vector<Pair<DistrictInfo, tg_geom*>>& districts);
