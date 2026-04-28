// Copyright 2025 itlab-vision

#include <gtest/gtest.h>
#include <vector>
#include <string>
#include <algorithm>
#include <stdexcept>

#include "find_district_neighbors.hpp"  // NOLINT
#include "district_info.hpp"            // NOLINT
#include "auxiliary.hpp"                // NOLINT

extern "C" {
#include "tg.h"  // NOLINT
}

class FindDistrictNeighborsTest : public ::testing::Test {
 protected:
    void SetUp() override {
        // District 1: (0,0)-(1,1)
        r1_geom = tg_parse_wkt("POLYGON((0 0, 1 0, 1 1, 0 1, 0 0))");
        if (tg_geom_error(r1_geom))
            throw std::runtime_error("Failed to parse D1 WKT");

        // District 2: (1,0)-(2,1) (Touches D1 at x=1)
        r2_geom = tg_parse_wkt("POLYGON((1 0, 2 0, 2 1, 1 1, 1 0))");
        if (tg_geom_error(r2_geom))
            throw std::runtime_error("Failed to parse D2 WKT");

        // District 3: (2,0)-(3,1) (Touches D2 at x=2, Disjoint from D1)
        r3_geom = tg_parse_wkt("POLYGON((2 0, 3 0, 3 1, 2 1, 2 0))");
        if (tg_geom_error(r3_geom))
            throw std::runtime_error("Failed to parse D3 WKT");

        // District 4: (10,10)-(11,11) (Disjoint from all)
        r4_geom = tg_parse_wkt("POLYGON((10 10, 11 10, 11 11, 10 11, 10 10))");
        if (tg_geom_error(r4_geom))
            throw std::runtime_error("Failed to parse D4 WKT");

        districts.push_back(
            {DistrictInfo(1, "District 1", "Регион 1"), r1_geom});
        districts.push_back(
            {DistrictInfo(2, "District 2", "Регион 2"), r2_geom});
        districts.push_back(
            {DistrictInfo(3, "District 3", "Регион 3"), r3_geom});
        districts.push_back(
            {DistrictInfo(4, "District 4", "Регион 4"), r4_geom});
    }

    void TearDown() override {
        if (r1_geom) tg_geom_free(r1_geom);
        if (r2_geom) tg_geom_free(r2_geom);
        if (r3_geom) tg_geom_free(r3_geom);
        if (r4_geom) tg_geom_free(r4_geom);
    }

    tg_geom* r1_geom = nullptr;
    tg_geom* r2_geom = nullptr;
    tg_geom* r3_geom = nullptr;
    tg_geom* r4_geom = nullptr;
    std::vector<Pair<DistrictInfo, tg_geom*>> districts;
};

// --- Empty Input Tests ---

TEST(FindDistrictNeighborsEmptyTest, ReturnsEmptyMap) {
    std::vector<Pair<DistrictInfo, tg_geom*>> empty_districts;
    auto result = find_district_neighbors(empty_districts);
    EXPECT_TRUE(result.empty());
}

// --- Single District Tests ---

TEST_F(FindDistrictNeighborsTest, SingleDistrict_MapSize) {
    std::vector<Pair<DistrictInfo, tg_geom*>> single_district = {districts[0]};
    auto result = find_district_neighbors(single_district);
    EXPECT_EQ(result.size(), 1);
}

TEST_F(FindDistrictNeighborsTest, SingleDistrict_HasEntryForId) {
    std::vector<Pair<DistrictInfo, tg_geom*>> single_district = {districts[0]};
    auto result = find_district_neighbors(single_district);
    EXPECT_TRUE(result.find(1) != result.end());
}

TEST_F(FindDistrictNeighborsTest, SingleDistrict_NoNeighbors) {
    std::vector<Pair<DistrictInfo, tg_geom*>> single_district = {districts[0]};
    auto result = find_district_neighbors(single_district);
    EXPECT_TRUE(result[1].empty());
}

// --- Complex Scenario Tests (4 districts) ---

TEST_F(FindDistrictNeighborsTest, Complex_MapSize) {
    auto result = find_district_neighbors(districts);
    EXPECT_EQ(result.size(), 4);
}

// District 1 neighbors
TEST_F(FindDistrictNeighborsTest, D1_HasEntry) {
    auto result = find_district_neighbors(districts);
    EXPECT_TRUE(result.find(1) != result.end());
}

TEST_F(FindDistrictNeighborsTest, D1_NeighborCount) {
    auto result = find_district_neighbors(districts);
    EXPECT_EQ(result[1].size(), 1);
}

TEST_F(FindDistrictNeighborsTest, D1_HasNeighbor_D2) {
    auto result = find_district_neighbors(districts);
    const auto& neighbors = result[1];
    EXPECT_NE(std::find(neighbors.begin(), neighbors.end(), 2),
              neighbors.end());
}

// District 2 neighbors
TEST_F(FindDistrictNeighborsTest, D2_HasEntry) {
    auto result = find_district_neighbors(districts);
    EXPECT_TRUE(result.find(2) != result.end());
}

TEST_F(FindDistrictNeighborsTest, D2_NeighborCount) {
    auto result = find_district_neighbors(districts);
    EXPECT_EQ(result[2].size(), 2);
}

TEST_F(FindDistrictNeighborsTest, D2_HasNeighbor_D1) {
    auto result = find_district_neighbors(districts);
    const auto& neighbors = result[2];
    EXPECT_NE(std::find(neighbors.begin(), neighbors.end(), 1),
              neighbors.end());
}

TEST_F(FindDistrictNeighborsTest, D2_HasNeighbor_D3) {
    auto result = find_district_neighbors(districts);
    const auto& neighbors = result[2];
    EXPECT_NE(std::find(neighbors.begin(), neighbors.end(), 3),
              neighbors.end());
}

// District 3 neighbors
TEST_F(FindDistrictNeighborsTest, D3_HasEntry) {
    auto result = find_district_neighbors(districts);
    EXPECT_TRUE(result.find(3) != result.end());
}

TEST_F(FindDistrictNeighborsTest, D3_NeighborCount) {
    auto result = find_district_neighbors(districts);
    EXPECT_EQ(result[3].size(), 1);
}

TEST_F(FindDistrictNeighborsTest, D3_HasNeighbor_D2) {
    auto result = find_district_neighbors(districts);
    const auto& neighbors = result[3];
    EXPECT_NE(std::find(neighbors.begin(), neighbors.end(), 2),
              neighbors.end());
}

// District 4 neighbors
TEST_F(FindDistrictNeighborsTest, D4_HasEntry) {
    auto result = find_district_neighbors(districts);
    EXPECT_TRUE(result.find(4) != result.end());
}

TEST_F(FindDistrictNeighborsTest, D4_NeighborCount) {
    auto result = find_district_neighbors(districts);
    EXPECT_TRUE(result[4].empty());
}

// --- Corner/Point Touching Test ---

TEST_F(FindDistrictNeighborsTest, PointTouching) {
    // Create two squares touching only at one corner (1,1)
    tg_geom* s1 = tg_parse_wkt("POLYGON((0 0, 1 0, 1 1, 0 1, 0 0))");
    tg_geom* s2 = tg_parse_wkt("POLYGON((1 1, 2 1, 2 2, 1 2, 1 1))");

    std::vector<Pair<DistrictInfo, tg_geom*>> point_districts;
    point_districts.push_back({DistrictInfo(10, "S1", "S1"), s1});
    point_districts.push_back({DistrictInfo(11, "S2", "S2"), s2});

    auto result = find_district_neighbors(point_districts);

    // Clean up manually since these are local
    tg_geom_free(s1);
    tg_geom_free(s2);

    // Check if they are neighbors
    bool are_neighbors = false;
    if (!result[10].empty()) {
        if (std::find(result[10].begin(), result[10].end(), 11) !=
            result[10].end()) {
            are_neighbors = true;
        }
    }

    EXPECT_TRUE(are_neighbors);
}
