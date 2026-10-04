// Copyright 2025 itlab-vision

#include <utility>
#include <vector>

#include "segments_converter.hpp"  // NOLINT

flatbuffers::Offset<SegmentsData::SegmentInfo> SegmentsConverter::convert_to_fb(
    flatbuffers::FlatBufferBuilder& builder, const SegmentInfo& segment) {
    auto segment_id = segment.segment_id;
    auto lat_range = SegmentsData::LatitudeRange(segment.lat_range.first,
                                                 segment.lat_range.second);
    auto lon_range = SegmentsData::LongitudeRange(segment.lon_range.first,
                                                  segment.lon_range.second);

    auto neighbors_vector = builder.CreateVector(segment.neighbors);
    auto locations_vector = builder.CreateVector(segment.points);

    auto segment_info_fb = SegmentsData::CreateSegmentInfo(
        builder, segment_id, &lat_range, &lon_range, neighbors_vector,
        locations_vector);

    return segment_info_fb;
}

SegmentInfo SegmentsConverter::convert_from_fb(
    const SegmentsData::SegmentInfo* segment) {
    std::vector<unsigned int> nbrs;
    if (auto fb_nbrs = segment->neighbors()) {
        nbrs.assign(fb_nbrs->begin(), fb_nbrs->end());
    }

    std::vector<unsigned int> locs;
    if (auto fb_locs = segment->points()) {
        locs.assign(fb_locs->begin(), fb_locs->end());
    }

    return SegmentInfo(
        segment->segment_id(),
        {segment->lat_range()->min(), segment->lat_range()->max()},
        {segment->lon_range()->min(), segment->lon_range()->max()},
        std::move(nbrs), std::move(locs));
}
