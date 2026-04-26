// Copyright 2025 itlab-vision
#pragma once
#include <chrono>

#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include "district_info.hpp"                 // NOLINT
#include "district_neighbors_generated.hpp"  // NOLINT

void print_district_neighbors(
    const flatbuffers::Vector<
        flatbuffers::Offset<DistrictNeighborsData::District>>* vect_neighbors);
