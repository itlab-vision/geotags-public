// Copyright 2025 itlab-vision
#pragma once

#include <vector>
#include <utility>
#include <type_traits>
#include <stdexcept>

#include "flatbuffers/flatbuffers.h"  // NOLINT

template <typename VectorType>
struct PointsWrapperTraits;

template <typename K, typename V>
struct PointsWrapperTraits<std::vector<std::pair<K, V>>> {
    using InfoType = K;
    using CoordType = V;
};

template <typename T>
struct PointsWrapperTraits<const flatbuffers::Vector<flatbuffers::Offset<T>>*> {
    using InfoType = decltype(std::declval<const T*>()->info());
    using CoordType = decltype(std::declval<const T*>()->coords());
};

template <typename VectorType>
class PointsWrapper {
 protected:
    VectorType data_;

 public:
    explicit PointsWrapper(VectorType data) : data_(std::move(data)) {}

    virtual ~PointsWrapper() = default;

    using InfoType = typename PointsWrapperTraits<VectorType>::InfoType;
    using CoordType = typename PointsWrapperTraits<VectorType>::CoordType;

    virtual size_t size() const = 0;
    virtual InfoType get_info(int idx) const = 0;
    virtual CoordType get_coord(int idx) const = 0;  // NOTE: Not used

    virtual double get_lat(int idx) const = 0;
    virtual double get_lon(int idx) const = 0;
};
