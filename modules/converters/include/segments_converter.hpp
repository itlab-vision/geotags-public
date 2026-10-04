// Copyright 2025 itlab-vision
#pragma once

#include <utility>
#include <vector>
#include <string>

#include "segments_generated.hpp"   // NOLINT
#include "segment_info.hpp"         // NOLINT
#include "converter_interface.hpp"  // NOLINT

class SegmentsConverter
    : public ConverterInterface<SegmentInfo, SegmentsData::SegmentInfo,
                                SegmentsData::Segments> {
 public:
    SegmentsConverter() = default;

    flatbuffers::Offset<SegmentsData::SegmentInfo> convert_to_fb(
        flatbuffers::FlatBufferBuilder& builder,
        const SegmentInfo& segment) override;

    SegmentInfo convert_from_fb(
        const SegmentsData::SegmentInfo* segment) override;

    flatbuffers::Offset<SegmentsData::Segments> create_root_fb(
        flatbuffers::FlatBufferBuilder& builder,
        flatbuffers::Offset<
            flatbuffers::Vector<flatbuffers::Offset<SegmentsData::SegmentInfo>>>
            data_vec) override {
        return SegmentsData::CreateSegments(builder, data_vec);
    }
};
