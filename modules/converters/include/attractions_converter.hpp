// Copyright 2025 itlab-vision
#pragma once

#include <utility>
#include <vector>
#include <string>

#include "attractions_generated.hpp"  // NOLINT
#include "attraction_info.hpp"        // NOLINT
#include "coordinates.hpp"            // NOLINT
#include "converter_interface.hpp"    // NOLINT

using AttractionItem = std::pair<AttractionInfo, Coordinates>;

class AttractionsConverter
    : public ConverterInterface<AttractionItem, AttractionsData::Attraction,
                                AttractionsData::Attractions> {
 public:
    AttractionsConverter() = default;

    flatbuffers::Offset<AttractionsData::Attraction> convert_to_fb(
        flatbuffers::FlatBufferBuilder& builder,
        const AttractionItem& attraction) override;

    AttractionItem convert_from_fb(
        const AttractionsData::Attraction* attraction) override;

    flatbuffers::Offset<AttractionsData::Attractions> create_root_fb(
        flatbuffers::FlatBufferBuilder& builder,
        flatbuffers::Offset<flatbuffers::Vector<
            flatbuffers::Offset<AttractionsData::Attraction>>>
            data_vec) override {
        return AttractionsData::CreateAttractions(builder, data_vec);
    }
};
