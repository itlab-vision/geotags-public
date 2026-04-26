// Copyright 2025 itlab-vision
#pragma once

#include <vector>
#include <tuple>
#include <utility>

#include "coordinates.hpp"             // NOLINT
#include "location_info.hpp"           // NOLINT
#include "locations_segment_info.hpp"  // NOLINT

std::tuple<std::vector<LocationsSegmentInfo>, int, int> compute_grid(
    const std::vector<std::pair<LocationInfo, Coordinates>> &locations,
    double cell_size);

double compute_optimal_cell_size(
    const std::vector<std::pair<LocationInfo, Coordinates>> &locations,
    int max_grid_size = 100);
