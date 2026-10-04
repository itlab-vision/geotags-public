// Copyright 2025 itlab-vision
#pragma once

#include <vector>
#include <utility>
#include <memory>

#include "points_wrapper.hpp"  // NOLINT

template <typename FBTable>
class PointsWrapperFB
    : public PointsWrapper<
          const flatbuffers::Vector<flatbuffers::Offset<FBTable>>*> {
 private:
    std::shared_ptr<char> buffer_;
    using FBVectorPtr =
        const flatbuffers::Vector<flatbuffers::Offset<FBTable>>*;

    uint16_t coords_struct_offset_ = 0;

 public:
    PointsWrapperFB(std::shared_ptr<char> buffer, FBVectorPtr data)
        : PointsWrapper<FBVectorPtr>(data), buffer_(std::move(buffer)) {
        // NOTE: Potential out-of-bounds access if the data array is empty
        // (Get(0)). We explicitly assume that the FlatBuffers vector is never
        // empty in this context.
        const FBTable* first_item = this->data_->Get(0);
        const auto* table =
            reinterpret_cast<const flatbuffers::Table*>(first_item);
        const auto* coords_ptr = first_item->coords();
        coords_struct_offset_ = reinterpret_cast<const uint8_t*>(coords_ptr) -
                                reinterpret_cast<const uint8_t*>(table);
    }

    size_t size() const override {
        return this->data_ ? this->data_->size() : 0;
    }

    typename PointsWrapperFB::InfoType get_info(int idx) const override {
        return this->data_->Get(idx)->info();
    }

    typename PointsWrapperFB::CoordType get_coord(int idx) const override {
        const FBTable* item = this->data_->Get(idx);
        return reinterpret_cast<typename PointsWrapperFB::CoordType>(
            reinterpret_cast<const uint8_t*>(item) + coords_struct_offset_);
    }

    double get_lat(int idx) const override {
        return get_coord(idx)->latitude();
    }

    double get_lon(int idx) const override {
        return get_coord(idx)->longitude();
    }
};
