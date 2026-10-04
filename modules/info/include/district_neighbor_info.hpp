// Copyright 2025 itlab-vision
#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <utility>

struct DistrictNeighborInfo {
    unsigned int district_id;
    std::vector<unsigned int> neighbors;

    DistrictNeighborInfo() {}
    DistrictNeighborInfo(unsigned int id, const std::vector<unsigned int>& nbrs)
        : district_id(id), neighbors(nbrs) {}

    friend std::ostream& operator<<(std::ostream& out,
                                    const DistrictNeighborInfo& info) {
        out << "District id " << info.district_id << " : ";
        bool first = true;
        for (const auto& val : info.neighbors) {
            if (!first) out << ", ";
            out << val;
            first = false;
        }
        out << "\n";
        return out;
    }
};
