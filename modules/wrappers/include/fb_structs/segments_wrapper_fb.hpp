// Copyright 2025 itlab-vision
#pragma once

#include <vector>
#include <utility>
#include <memory>

#include "segments_wrapper.hpp"    // NOLINT
#include "segments_generated.hpp"  // NOLINT

class SegmentsWrapperFB
    : public SegmentsWrapper<const flatbuffers::Vector<
          flatbuffers::Offset<SegmentsData::SegmentInfo>>*> {
 private:
    std::shared_ptr<char> buffer_;
    using FBVectorPtr = const flatbuffers::Vector<
        flatbuffers::Offset<SegmentsData::SegmentInfo>>*;

 public:
    SegmentsWrapperFB(std::shared_ptr<char> buffer, FBVectorPtr data)
        : SegmentsWrapper<FBVectorPtr>(data), buffer_(std::move(buffer)) {}

    size_t size() const override {
        return this->data_ ? this->data_->size() : 0;
    }

    unsigned int get_segment_id(int idx) const override {
        return this->data_->Get(idx)->segment_id();
    }

    double get_lat_min(int idx) const override {
        return this->data_->Get(idx)->lat_range()->min();
    }

    double get_lat_max(int idx) const override {
        return this->data_->Get(idx)->lat_range()->max();
    }

    double get_lon_min(int idx) const override {
        return this->data_->Get(idx)->lon_range()->min();
    }

    double get_lon_max(int idx) const override {
        return this->data_->Get(idx)->lon_range()->max();
    }

    void get_nbrs(int idx, std::vector<unsigned int>& out) const override {
        auto fb_vec = this->data_->Get(idx)->neighbors();
        if (!fb_vec) {
            out.clear();
        } else {
            out.assign(fb_vec->begin(), fb_vec->end());
        }
    }

    void get_points(int segment_idx,
                    std::vector<unsigned int>& out) const override {
        auto fb_vec = this->data_->Get(segment_idx)->points();
        if (!fb_vec) {
            out.clear();
        } else {
            out.assign(fb_vec->begin(), fb_vec->end());
        }
    }
};
