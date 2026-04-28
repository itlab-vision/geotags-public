// Copyright 2025 itlab-vision

#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include <unordered_map>

#include "district_info.hpp"             // NOLINT
#include "print_district_neighbors.hpp"  // NOLINT

void print_district_neighbors(
    const std::unordered_map<unsigned int, std::vector<unsigned int>>&
        map_neighbors) {
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
