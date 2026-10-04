// Copyright 2025 itlab-vision
#pragma once

#include <vector>
#include <utility>
#include <type_traits>
#include <stdexcept>

#include "flatbuffers/flatbuffers.h"  // NOLINT

template <typename VectorType>
class SegmentsWrapper {
 protected:
    VectorType data_;

 public:
    explicit SegmentsWrapper(VectorType data) : data_(std::move(data)) {}

    virtual ~SegmentsWrapper() = default;

    virtual size_t size() const = 0;
    virtual unsigned int get_segment_id(int idx) const = 0;
    virtual double get_lat_min(int idx) const = 0;
    virtual double get_lat_max(int idx) const = 0;
    virtual double get_lon_min(int idx) const = 0;
    virtual double get_lon_max(int idx) const = 0;

    virtual void get_nbrs(int idx, std::vector<unsigned int>& out) const = 0;
    virtual void get_points(int segment_idx,
                            std::vector<unsigned int>& out) const = 0;
};
