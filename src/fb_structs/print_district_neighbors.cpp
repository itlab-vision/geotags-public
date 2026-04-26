// Copyright 2025 itlab-vision

#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include "district_info.hpp"              // NOLINT
#include "print_district_neighbors.hpp"   // NOLINT
#include "district_neighbors_reader.hpp"  // NOLINT

void print_district_neighbors(
    const flatbuffers::Vector<
        flatbuffers::Offset<DistrictNeighborsData::District>>* vect_neighbors) {
    if (!vect_neighbors || !vect_neighbors) {
        std::cout << "No district data available" << std::endl;
        return;
    }

    for (flatbuffers::uoffset_t i = 0; i < vect_neighbors->size(); i++) {
        const auto district = vect_neighbors->Get(i);

        std::cout << "District id " << district->id() << " : ";

        auto neighbors = district->neighbors();
        if (!neighbors || neighbors->size() == 0) {
            std::cout << "have no neighbors";
        } else {
            for (flatbuffers::uoffset_t j = 0; j < neighbors->size(); j++) {
                if (j > 0) std::cout << ", ";
                std::cout << neighbors->Get(j);
            }
        }
        std::cout << std::endl;
    }
}
