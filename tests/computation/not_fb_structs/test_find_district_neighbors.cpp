// Copyright 2025 itlab-vision
#include <gtest/gtest.h>
#include <vector>
#include <string>
#include <algorithm>
#include <map>
#include <memory>

#include "district_neighbors_utils.hpp"  // NOLINT
#include "districts_wrapper_stl.hpp"     // NOLINT
#include "district_info.hpp"             // NOLINT
#include "auxiliary.hpp"                 // NOLINT

extern "C" {
#include "tg.h"  // NOLINT
}

class FindDistrictNeighborsTest : public ::testing::Test {
 protected:
    void SetUp() override {
        // District 1: (0,0)-(1,1)
        r1_geom = TgGeomSharedPtr(
            TgGeomPtr(tg_parse_wkt("POLYGON((0 0, 1 0, 1 1, 0 1, 0 0))")));
        // District 2: (1,0)-(2,1) (Touches D1 at x=1)
        r2_geom = TgGeomSharedPtr(
            TgGeomPtr(tg_parse_wkt("POLYGON((1 0, 2 0, 2 1, 1 1, 1 0))")));
        // District 3: (2,0)-(3,1) (Touches D2 at x=2, Disjoint from D1)
        r3_geom = TgGeomSharedPtr(
            TgGeomPtr(tg_parse_wkt("POLYGON((2 0, 3 0, 3 1, 2 1, 2 0))")));
        // District 4: (10,10)-(11,11) (Disjoint from all)
        r4_geom = TgGeomSharedPtr(TgGeomPtr(
            tg_parse_wkt("POLYGON((10 10, 11 10, 11 11, 10 11, 10 10))")));

        DistrictItem item1{DistrictInfo(1, "District 1", "Регион 1"), r1_geom};
        DistrictItem item2{DistrictInfo(2, "District 2", "Регион 2"), r2_geom};
        DistrictItem item3{DistrictInfo(3, "District 3", "Регион 3"), r3_geom};
        DistrictItem item4{DistrictInfo(4, "District 4", "Регион 4"), r4_geom};

        std::vector<DistrictItem> vec = {item1, item2, item3, item4};
        districts = std::make_unique<DistrictsWrapperSTL>(vec);
    }

    TgGeomSharedPtr r1_geom = nullptr;
    TgGeomSharedPtr r2_geom = nullptr;
    TgGeomSharedPtr r3_geom = nullptr;
    TgGeomSharedPtr r4_geom = nullptr;
    std::unique_ptr<DistrictsWrapperSTL> districts;
};

TEST_F(FindDistrictNeighborsTest, EmptyInput_ReturnsEmptyMap) {
    std::vector<DistrictItem> empty_vec;
    DistrictsWrapperSTL empty_districts(empty_vec);
    auto result = find_district_neighbors(empty_districts);
    EXPECT_TRUE(result.empty());
}

TEST_F(FindDistrictNeighborsTest, SingleDistrict_MapSize) {
    DistrictItem item1{DistrictInfo(1, "District 1", "Регион 1"), r1_geom};
    std::vector<DistrictItem> vec = {item1};
    DistrictsWrapperSTL single_district(vec);
    auto result = find_district_neighbors(single_district);
    EXPECT_EQ(result.size(), 1u);
}

TEST_F(FindDistrictNeighborsTest, SingleDistrict_HasNoNeighbors) {
    DistrictItem item1{DistrictInfo(1, "District 1", "Регион 1"), r1_geom};
    std::vector<DistrictItem> vec = {item1};
    DistrictsWrapperSTL single_district(vec);
    auto result = find_district_neighbors(single_district);
    EXPECT_TRUE(result[1].empty());
}

TEST_F(FindDistrictNeighborsTest, Complex_MapSize) {
    auto result = find_district_neighbors(*districts);
    EXPECT_EQ(result.size(), 4u);
}

TEST_F(FindDistrictNeighborsTest, D1_NeighborCount) {
    auto result = find_district_neighbors(*districts);
    EXPECT_EQ(result[1].size(), 1u);
}

TEST_F(FindDistrictNeighborsTest, D1_HasNeighbor_D2) {
    auto result = find_district_neighbors(*districts);
    const auto& neighbors = result[1];
    EXPECT_NE(std::find(neighbors.begin(), neighbors.end(), 2u),
              neighbors.end());
}

TEST_F(FindDistrictNeighborsTest, D2_NeighborCount) {
    auto result = find_district_neighbors(*districts);
    EXPECT_EQ(result[2].size(), 2u);
}

TEST_F(FindDistrictNeighborsTest, D2_HasNeighbor_D1) {
    auto result = find_district_neighbors(*districts);
    const auto& neighbors = result[2];
    EXPECT_NE(std::find(neighbors.begin(), neighbors.end(), 1u),
              neighbors.end());
}

TEST_F(FindDistrictNeighborsTest, D2_HasNeighbor_D3) {
    auto result = find_district_neighbors(*districts);
    const auto& neighbors = result[2];
    EXPECT_NE(std::find(neighbors.begin(), neighbors.end(), 3u),
              neighbors.end());
}

TEST_F(FindDistrictNeighborsTest, D4_NeighborCount) {
    auto result = find_district_neighbors(*districts);
    EXPECT_TRUE(result[4].empty());
}

TEST_F(FindDistrictNeighborsTest, PointTouching_AreNeighbors) {
    TgGeomSharedPtr s1(
        TgGeomPtr(tg_parse_wkt("POLYGON((0 0, 1 0, 1 1, 0 1, 0 0))")));
    TgGeomSharedPtr s2(
        TgGeomPtr(tg_parse_wkt("POLYGON((1 1, 2 1, 2 2, 1 2, 1 1))")));

    std::vector<DistrictItem> point_districts_vec;
    point_districts_vec.push_back({DistrictInfo(10, "S1", "S1"), s1});
    point_districts_vec.push_back({DistrictInfo(11, "S2", "S2"), s2});
    DistrictsWrapperSTL point_districts(point_districts_vec);

    auto result = find_district_neighbors(point_districts);
    const auto& neighbors = result[10];
    EXPECT_NE(std::find(neighbors.begin(), neighbors.end(), 11u),
              neighbors.end());
}
