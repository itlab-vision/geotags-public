// Copyright 2025 itlab-vision
#pragma once

#include <vector>

#include "district_neighbors_wrapper.hpp"  // NOLINT
#include "district_neighbor_info.hpp"      // NOLINT

class DistrictNeighborsWrapperSTL
    : public DistrictNeighborsWrapper<std::vector<DistrictNeighborInfo>> {
 private:
    using DistrictNeighborsWrapper<
        std::vector<DistrictNeighborInfo>>::DistrictNeighborsWrapper;

 public:
    size_t size() const override { return this->data_.size(); }

    void get_nbrs(int idx, std::vector<unsigned int>& out) const override {
        out = this->data_[idx].neighbors;
    }
};
