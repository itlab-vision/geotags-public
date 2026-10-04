// Copyright 2025 itlab-vision
#pragma once

#include <vector>
#include <utility>
#include <memory>

#include "districts_wrapper.hpp"    // NOLINT
#include "districts_generated.hpp"  // NOLINT
#include "districts_utils.hpp"      // NOLINT

class DistrictsWrapperFB
    : public DistrictsWrapper<const flatbuffers::Vector<
          flatbuffers::Offset<DistrictsData::DistrictEntry>>*> {
 private:
    std::shared_ptr<char> buffer_;
    using FBVectorPtr = const flatbuffers::Vector<
        flatbuffers::Offset<DistrictsData::DistrictEntry>>*;

    mutable std::vector<TgGeomSharedPtr> geom_cache_;

 public:
    DistrictsWrapperFB(std::shared_ptr<char> buffer, FBVectorPtr data)
        : DistrictsWrapper<FBVectorPtr>(data), buffer_(std::move(buffer)) {
        if (this->data_) {
            geom_cache_.resize(this->data_->size());
        }
    }

    size_t size() const override {
        return this->data_ ? this->data_->size() : 0;
    }

    DistrictInfo get_info(int idx) const override {
        return DistrictInfo(this->data_->Get(idx)->info());
    }

    unsigned int get_info_id(int idx) const override {
        return this->data_->Get(idx)->info()->id();
    };

    TgGeomSharedPtr get_geom(int idx) const override {
        if (!geom_cache_[idx]) {
            auto geom = deserialize_geom(this->data_->Get(idx)->geom_type(),
                                         this->data_->Get(idx)->geom());
            geom_cache_[idx] = TgGeomSharedPtr(std::move(geom));
        }
        return geom_cache_[idx];
    }
};
