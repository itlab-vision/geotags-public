// Copyright 2025 itlab-vision
#include <gtest/gtest.h>
#include <vector>

#include "points_wrapper_stl.hpp"  // NOLINT
#include "location_info.hpp"       // NOLINT
#include "coordinates.hpp"         // NOLINT

class PointsWrapperSTLTest : public ::testing::Test {
 protected:
    std::vector<std::pair<LocationInfo, Coordinates>> data;
    PointsWrapperSTL<std::pair<LocationInfo, Coordinates>>* wrapper;

    void SetUp() override {
        LocationInfo info1{1, "RU", "MOW", 2, 3, {}};
        Coordinates coords1{55.75, 37.61};

        LocationInfo info2{2, "RU", "SPB", 4, 5, {}};
        Coordinates coords2{59.93, 30.33};

        data.push_back({info1, coords1});
        data.push_back({info2, coords2});

        wrapper =
            new PointsWrapperSTL<std::pair<LocationInfo, Coordinates>>(data);
    }

    void TearDown() override { delete wrapper; }
};

TEST_F(PointsWrapperSTLTest, SizeCorrect) { EXPECT_EQ(wrapper->size(), 2u); }
TEST_F(PointsWrapperSTLTest, GetInfoCorrect) {
    EXPECT_EQ(wrapper->get_info(0).location_id, 1u);
}
TEST_F(PointsWrapperSTLTest, GetLatFirstCorrect) {
    EXPECT_DOUBLE_EQ(wrapper->get_lat(0), 55.75);
}
TEST_F(PointsWrapperSTLTest, GetLonFirstCorrect) {
    EXPECT_DOUBLE_EQ(wrapper->get_lon(0), 37.61);
}
TEST_F(PointsWrapperSTLTest, GetLatSecondCorrect) {
    EXPECT_DOUBLE_EQ(wrapper->get_lat(1), 59.93);
}
TEST_F(PointsWrapperSTLTest, GetLonSecondCorrect) {
    EXPECT_DOUBLE_EQ(wrapper->get_lon(1), 30.33);
}
