// Copyright 2025 itlab-vision
#pragma once

#include <vector>

#include "points_wrapper.hpp"  // NOLINT

template <typename ItemType>  // sample: `std::pair<LocationInfo, Coordinates>`
class PointsWrapperSTL : public PointsWrapper<std::vector<ItemType>> {
 private:
    using PointsWrapper<std::vector<ItemType>>::PointsWrapper;

 public:
    size_t size() const override { return this->data_.size(); }

    typename PointsWrapperSTL::InfoType get_info(int idx) const override {
        return this->data_[idx].first;
    }

    typename PointsWrapperSTL::CoordType get_coord(int idx) const override {
        return this->data_[idx].second;
    }

    double get_lat(int idx) const override {
        return this->data_[idx].second.latitude;
    }

    double get_lon(int idx) const override {
        return this->data_[idx].second.longitude;
    }
};
