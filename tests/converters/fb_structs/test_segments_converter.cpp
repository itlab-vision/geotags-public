// Copyright 2025 itlab-vision
#include <gtest/gtest.h>
#include <string>
#include <vector>
#include <utility>

#include "flatbuffers/flatbuffers.h"  // NOLINT
#include "segments_converter.hpp"     // NOLINT

class SegmentsConverterTest : public ::testing::Test {
 protected:
    SegmentInfo restored_item;
    SegmentsConverter converter{};

    void SetUp() override {
        SegmentInfo original_item;
        original_item.segment_id = 22;
        original_item.lat_range = {1.0, 2.0};
        original_item.lon_range = {3.0, 4.0};
        original_item.neighbors = {30, 31};
        original_item.points = {100, 101, 102};

        flatbuffers::FlatBufferBuilder builder;
        auto offset = converter.convert_to_fb(builder, original_item);
        std::vector<flatbuffers::Offset<SegmentsData::SegmentInfo>> vec = {
            offset};
        auto vec_offset = builder.CreateVector(vec);
        auto root = converter.create_root_fb(builder, vec_offset);
        builder.Finish(root);

        const auto* root_fb =
            SegmentsData::GetSegments(builder.GetBufferPointer());
        const auto* item_fb = root_fb->data()->Get(0);
        restored_item = converter.convert_from_fb(item_fb);
    }
};

TEST_F(SegmentsConverterTest, SegmentIdCorrect) {
    EXPECT_EQ(restored_item.segment_id, 22u);
}
TEST_F(SegmentsConverterTest, LatRangeFirstCorrect) {
    EXPECT_DOUBLE_EQ(restored_item.lat_range.first, 1.0);
}
TEST_F(SegmentsConverterTest, LatRangeSecondCorrect) {
    EXPECT_DOUBLE_EQ(restored_item.lat_range.second, 2.0);
}
TEST_F(SegmentsConverterTest, LonRangeFirstCorrect) {
    EXPECT_DOUBLE_EQ(restored_item.lon_range.first, 3.0);
}
TEST_F(SegmentsConverterTest, LonRangeSecondCorrect) {
    EXPECT_DOUBLE_EQ(restored_item.lon_range.second, 4.0);
}
TEST_F(SegmentsConverterTest, NeighborsSizeCorrect) {
    EXPECT_EQ(restored_item.neighbors.size(), 2u);
}
TEST_F(SegmentsConverterTest, NeighborsFirstElementCorrect) {
    EXPECT_EQ(restored_item.neighbors[0], 30u);
}
TEST_F(SegmentsConverterTest, PointsSizeCorrect) {
    EXPECT_EQ(restored_item.points.size(), 3u);
}
TEST_F(SegmentsConverterTest, PointsFirstElementCorrect) {
    EXPECT_EQ(restored_item.points[0], 100u);
}
