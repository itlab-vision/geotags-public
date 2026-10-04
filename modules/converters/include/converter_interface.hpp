// Copyright 2025 itlab-vision
#pragma once

#include <string>
#include <vector>

#include "flatbuffers/flatbuffers.h"  // NOLINT

template <typename Item, typename FBItem, typename FBRoot>
class ConverterInterface {
 public:
    using ItemType = Item;
    using FBRootType = FBRoot;

    ConverterInterface() = default;
    virtual ~ConverterInterface() = default;

    ConverterInterface(const ConverterInterface&) = delete;
    ConverterInterface& operator=(const ConverterInterface&) = delete;
    ConverterInterface(ConverterInterface&&) = delete;
    ConverterInterface& operator=(ConverterInterface&&) = delete;

    virtual flatbuffers::Offset<FBItem> convert_to_fb(
        flatbuffers::FlatBufferBuilder& builder, const Item& item) = 0;

    virtual Item convert_from_fb(const FBItem* fb_item) = 0;

    virtual flatbuffers::Offset<FBRoot> create_root_fb(
        flatbuffers::FlatBufferBuilder& builder,
        flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<FBItem>>>
            data_vec) = 0;
};
