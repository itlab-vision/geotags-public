// Copyright 2025 itlab-vision
#pragma once
#include <chrono>

#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include <map>

extern "C" {
#include "tg.h"  // NOLINT
}

template <typename DistrictsWrapper>
std::map<unsigned int, std::vector<unsigned int>> find_district_neighbors(
    const DistrictsWrapper& districts) {
    std::map<unsigned int, std::vector<unsigned int>> neighbors_map;
    for (size_t i = 0; i < districts.size(); ++i) {
        neighbors_map[districts.get_info_id(i)] = {};
    }

    for (size_t i = 0; i < districts.size(); ++i) {
        unsigned int id_i = districts.get_info_id(i);

        for (size_t j = i + 1; j < districts.size(); ++j) {
            if (tg_geom_touches(districts.get_geom(i).get(),
                                districts.get_geom(j).get())) {
                unsigned int id_j = districts.get_info_id(j);
                neighbors_map[id_i].push_back(id_j);
                neighbors_map[id_j].push_back(id_i);
            }
        }
    }
    return neighbors_map;
}

void print_district_neighbors(
    const std::map<unsigned int, std::vector<unsigned int>>& map_neighbors) {
    for (const auto& pair : map_neighbors) {
        unsigned int district_id = pair.first;
        const std::vector<unsigned int> neighbors = pair.second;

        std::cout << "District id " << district_id << ": ";
        if (neighbors.empty()) {
            std::cout << "have no neighbors";
        } else {
            for (size_t j = 0; j < neighbors.size(); ++j) {
                std::cout << neighbors[j];
                if (j < neighbors.size() - 1) {
                    std::cout << ", ";
                }
            }
        }
        std::cout << std::endl;
    }
}
