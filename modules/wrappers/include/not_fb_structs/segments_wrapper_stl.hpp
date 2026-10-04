// Copyright 2025 itlab-vision
#pragma once

#include <vector>

#include "segments_wrapper.hpp"  // NOLINT
#include "segment_info.hpp"      // NOLINT

class SegmentsWrapperSTL : public SegmentsWrapper<std::vector<SegmentInfo>> {
 private:
    using SegmentsWrapper<std::vector<SegmentInfo>>::SegmentsWrapper;

 public:
    size_t size() const override { return this->data_.size(); }

    unsigned int get_segment_id(int idx) const override {
        return this->data_[idx].segment_id;
    }

    double get_lat_min(int idx) const override {
        return this->data_[idx].lat_range.first;
    }

    double get_lat_max(int idx) const override {
        return this->data_[idx].lat_range.second;
    }

    double get_lon_min(int idx) const override {
        return this->data_[idx].lon_range.first;
    }

    double get_lon_max(int idx) const override {
        return this->data_[idx].lon_range.second;
    }

    void get_nbrs(int idx, std::vector<unsigned int>& out) const override {
        out = this->data_[idx].neighbors;
    }

    void get_points(int segment_idx,
                    std::vector<unsigned int>& out) const override {
        out = this->data_[segment_idx].points;
    }
};
