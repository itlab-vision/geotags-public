// Copyright 2025 itlab-vision
#pragma once

#include <vector>
#include <utility>

#include "districts_wrapper.hpp"  // NOLINT

extern "C" {
#include "tg.h"  // NOLINT
}

using DistrictItem = std::pair<DistrictInfo, TgGeomSharedPtr>;

class DistrictsWrapperSTL : public DistrictsWrapper<std::vector<DistrictItem>> {
 private:
    using DistrictsWrapper<std::vector<DistrictItem>>::DistrictsWrapper;

 public:
    size_t size() const override { return this->data_.size(); }

    DistrictInfo get_info(int idx) const override {
        return this->data_[idx].first;
    }

    unsigned int get_info_id(int idx) const override {
        return this->data_[idx].first.id;
    };

    TgGeomSharedPtr get_geom(int idx) const override {
        return this->data_[idx].second;
    }
};
