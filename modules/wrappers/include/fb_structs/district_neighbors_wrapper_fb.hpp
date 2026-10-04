// Copyright 2025 itlab-vision
#pragma once

#include <vector>
#include <utility>
#include <memory>

#include "district_neighbors_generated.hpp"  // NOLINT
#include "district_neighbors_wrapper.hpp"    // NOLINT

class DistrictNeighborsWrapperFB
    : public DistrictNeighborsWrapper<const flatbuffers::Vector<
          flatbuffers::Offset<DistrictNeighborsData::DistrictNeighborInfo>>*> {
 private:
    std::shared_ptr<char> buffer_;
    using FBVectorPtr = const flatbuffers::Vector<
        flatbuffers::Offset<DistrictNeighborsData::DistrictNeighborInfo>>*;

 public:
    DistrictNeighborsWrapperFB(std::shared_ptr<char> buffer, FBVectorPtr data)
        : DistrictNeighborsWrapper<FBVectorPtr>(data),
          buffer_(std::move(buffer)) {}

    size_t size() const override {
        return this->data_ ? this->data_->size() : 0;
    }

    void get_nbrs(int idx, std::vector<unsigned int>& out) const override {
        auto fb_vec = this->data_->Get(idx)->neighbors();
        if (!fb_vec) {
            out.clear();
        } else {
            out.assign(fb_vec->begin(), fb_vec->end());
        }
    }
};
