// Copyright 2025 itlab-vision
#include <gtest/gtest.h>
#include <vector>

#include "district_neighbors_wrapper_stl.hpp"  // NOLINT

class DistrictNeighborsWrapperSTLTest : public ::testing::Test {
 protected:
    std::vector<DistrictNeighborInfo> data;
    DistrictNeighborsWrapperSTL* wrapper;

    void SetUp() override {
        data.push_back(DistrictNeighborInfo{5, {6, 7}});
        data.push_back(DistrictNeighborInfo{8, {9}});
        wrapper = new DistrictNeighborsWrapperSTL(data);
    }

    void TearDown() override { delete wrapper; }
};

TEST_F(DistrictNeighborsWrapperSTLTest, SizeCorrect) {
    EXPECT_EQ(wrapper->size(), 2u);
}
TEST_F(DistrictNeighborsWrapperSTLTest, NeighborsSizeCorrect) {
    std::vector<unsigned int> nbrs;
    wrapper->get_nbrs(0, nbrs);
    EXPECT_EQ(nbrs.size(), 2u);
}
TEST_F(DistrictNeighborsWrapperSTLTest, NeighborsFirstElementCorrect) {
    std::vector<unsigned int> nbrs;
    wrapper->get_nbrs(0, nbrs);
    EXPECT_EQ(nbrs[0], 6u);
}
