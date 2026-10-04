// Copyright 2025 itlab-vision
#include <gtest/gtest.h>
#include <cstring>
#include <memory>
#include <vector>
#include <utility>

#include "flatbuffers/flatbuffers.h"  // NOLINT
#include "segments_wrapper_fb.hpp"    // NOLINT
#include "segments_converter.hpp"     // NOLINT

class SegmentsWrapperFBTest : public ::testing::Test {
 protected:
    SegmentsWrapperFB* wrapper;

    void SetUp() override {
        SegmentsConverter converter{};

        SegmentInfo item;
        item.segment_id = 10;
        item.lat_range = {1.0, 2.0};
        item.lon_range = {3.0, 4.0};
        item.neighbors = {11};
        item.points = {100};

        flatbuffers::FlatBufferBuilder builder;
        auto offset = converter.convert_to_fb(builder, item);
        std::vector<flatbuffers::Offset<SegmentsData::SegmentInfo>> vec = {
            offset};
        auto vec_offset = builder.CreateVector(vec);
        auto root = converter.create_root_fb(builder, vec_offset);
        builder.Finish(root);

        const uint8_t* buf = builder.GetBufferPointer();
        size_t size = builder.GetSize();
        std::shared_ptr<char> buffer(new char[size],
                                     std::default_delete<char[]>());
        std::memcpy(buffer.get(), buf, size);

        const auto* root_fb = SegmentsData::GetSegments(buffer.get());
        wrapper = new SegmentsWrapperFB(buffer, root_fb->data());
    }

    void TearDown() override { delete wrapper; }
};

TEST_F(SegmentsWrapperFBTest, SizeCorrect) { EXPECT_EQ(wrapper->size(), 1u); }
TEST_F(SegmentsWrapperFBTest, SegmentIdCorrect) {
    EXPECT_EQ(wrapper->get_segment_id(0), 10u);
}
TEST_F(SegmentsWrapperFBTest, LatMinCorrect) {
    EXPECT_DOUBLE_EQ(wrapper->get_lat_min(0), 1.0);
}
TEST_F(SegmentsWrapperFBTest, LatMaxCorrect) {
    EXPECT_DOUBLE_EQ(wrapper->get_lat_max(0), 2.0);
}
TEST_F(SegmentsWrapperFBTest, LonMinCorrect) {
    EXPECT_DOUBLE_EQ(wrapper->get_lon_min(0), 3.0);
}
TEST_F(SegmentsWrapperFBTest, LonMaxCorrect) {
    EXPECT_DOUBLE_EQ(wrapper->get_lon_max(0), 4.0);
}
TEST_F(SegmentsWrapperFBTest, NeighborsSizeCorrect) {
    std::vector<unsigned int> nbrs;
    wrapper->get_nbrs(0, nbrs);
    EXPECT_EQ(nbrs.size(), 1u);
}
TEST_F(SegmentsWrapperFBTest, PointsSizeCorrect) {
    std::vector<unsigned int> pts;
    wrapper->get_points(0, pts);
    EXPECT_EQ(pts.size(), 1u);
}
