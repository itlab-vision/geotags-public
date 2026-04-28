// Copyright 2025 itlab-vision

#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include <unordered_map>

#include "auxiliary.hpp"                // NOLINT
#include "district_info.hpp"            // NOLINT
#include "find_district_neighbors.hpp"  // NOLINT

std::unordered_map<unsigned int, std::vector<unsigned int>>
find_district_neighbors(
    const std::vector<Pair<DistrictInfo, tg_geom*>>& districts) {
    std::unordered_map<unsigned int, std::vector<unsigned int>> neighbors_map;
    for (const auto& district : districts) {
        neighbors_map[district.first.id] = {};
    }
    for (size_t i = 0; i < districts.size(); ++i) {
        unsigned int id_i = districts[i].first.id;

        for (size_t j = i + 1; j < districts.size(); ++j) {
            if (tg_geom_touches(districts[i].second, districts[j].second)) {
                unsigned int id_j = districts[j].first.id;
                neighbors_map[id_i].push_back(id_j);
                neighbors_map[id_j].push_back(id_i);
            }
        }
    }
    return neighbors_map;
}
