// Copyright 2025 itlab-vision
#include <gtest/gtest.h>
#include <vector>

#include "segments_wrapper_stl.hpp"  // NOLINT

class SegmentsWrapperSTLTest : public ::testing::Test {
 protected:
    std::vector<SegmentInfo> data;
    SegmentsWrapperSTL* wrapper;

    void SetUp() override {
        data.push_back(SegmentInfo{10, {1.0, 2.0}, {3.0, 4.0}, {11}, {100}});
        wrapper = new SegmentsWrapperSTL(data);
    }

    void TearDown() override { delete wrapper; }
};

TEST_F(SegmentsWrapperSTLTest, SizeCorrect) { EXPECT_EQ(wrapper->size(), 1u); }
TEST_F(SegmentsWrapperSTLTest, SegmentIdCorrect) {
    EXPECT_EQ(wrapper->get_segment_id(0), 10u);
}
TEST_F(SegmentsWrapperSTLTest, LatMinCorrect) {
    EXPECT_DOUBLE_EQ(wrapper->get_lat_min(0), 1.0);
}
TEST_F(SegmentsWrapperSTLTest, LatMaxCorrect) {
    EXPECT_DOUBLE_EQ(wrapper->get_lat_max(0), 2.0);
}
TEST_F(SegmentsWrapperSTLTest, LonMinCorrect) {
    EXPECT_DOUBLE_EQ(wrapper->get_lon_min(0), 3.0);
}
TEST_F(SegmentsWrapperSTLTest, LonMaxCorrect) {
    EXPECT_DOUBLE_EQ(wrapper->get_lon_max(0), 4.0);
}
TEST_F(SegmentsWrapperSTLTest, NeighborsSizeCorrect) {
    std::vector<unsigned int> nbrs;
    wrapper->get_nbrs(0, nbrs);
    EXPECT_EQ(nbrs.size(), 1u);
}
TEST_F(SegmentsWrapperSTLTest, PointsSizeCorrect) {
    std::vector<unsigned int> pts;
    wrapper->get_points(0, pts);
    EXPECT_EQ(pts.size(), 1u);
}
