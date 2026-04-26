// Copyright 2025 itlab-vision
#pragma once
#include <chrono>

#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include <unordered_map>

#include "district_info.hpp"  // NOLINT

void print_district_neighbors(
    const std::unordered_map<unsigned int, std::vector<unsigned int>>&
        map_neighbors);
