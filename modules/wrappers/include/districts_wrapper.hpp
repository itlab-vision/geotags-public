// Copyright 2025 itlab-vision
#pragma once

#include <vector>
#include <utility>
#include <type_traits>
#include <stdexcept>

#include "flatbuffers/flatbuffers.h"  // NOLINT
#include "district_info.hpp"          // NOLINT
#include "auxiliary.hpp"              // NOLINT

template <typename VectorType>
class DistrictsWrapper {
 protected:
    VectorType data_;

 public:
    explicit DistrictsWrapper(VectorType data) : data_(std::move(data)) {}

    virtual ~DistrictsWrapper() = default;

    virtual size_t size() const = 0;

    virtual DistrictInfo get_info(int idx) const = 0;
    virtual unsigned int get_info_id(int idx) const = 0;
    virtual TgGeomSharedPtr get_geom(int idx) const = 0;
};
