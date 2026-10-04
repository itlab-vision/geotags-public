// Copyright 2025 itlab-vision
#pragma once

#include <vector>
#include <utility>
#include <type_traits>
#include <stdexcept>

#include "flatbuffers/flatbuffers.h"  // NOLINT

template <typename VectorType>
class DistrictNeighborsWrapper {
 protected:
    VectorType data_;

 public:
    explicit DistrictNeighborsWrapper(VectorType data)
        : data_(std::move(data)) {}

    virtual ~DistrictNeighborsWrapper() = default;

    virtual size_t size() const = 0;
    virtual void get_nbrs(int idx, std::vector<unsigned int>& out) const = 0;
};
