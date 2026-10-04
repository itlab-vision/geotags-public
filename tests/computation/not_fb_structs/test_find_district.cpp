// Copyright 2025 itlab-vision
#include <gtest/gtest.h>
#include <memory>
#include <vector>
#include <string>
#include <stdexcept>
#include <utility>
#include <optional>

#include "district_finder.hpp"                 // NOLINT
#include "coordinates.hpp"                     // NOLINT
#include "location_info.hpp"                   // NOLINT
#include "district_info.hpp"                   // NOLINT
#include "districts_wrapper_stl.hpp"           // NOLINT
#include "district_neighbors_wrapper_stl.hpp"  // NOLINT

extern "C" {
#include "tg.h"  // NOLINT
}

class FindDistrictTest : public ::testing::Test {
 protected:
    void SetUp() override {
        // Create test polygons
        moscow_district_geom =
            TgGeomPtr(tg_parse_wkt("POLYGON((35.0 55.0, 37.0 55.0, 37.0 57.0, "
                                   "35.0 57.0, 35.0 55.0))"));
        tver_district_geom =
            TgGeomPtr(tg_parse_wkt("POLYGON((40.0 50.0, 42.0 50.0, 42.0 52.0, "
                                   "40.0 52.0, 40.0 50.0))"));

        DistrictItem item1;
        item1.first = DistrictInfo(1, "Moscow Oblast", "Московская область");
        item1.second = TgGeomSharedPtr(std::move(moscow_district_geom));

        DistrictItem item2;
        item2.first = DistrictInfo(2, "Tver Oblast", "Тверская область");
        item2.second = TgGeomSharedPtr(std::move(tver_district_geom));

        std::vector<DistrictItem> districts_vec;
        districts_vec.push_back(std::move(item1));
        districts_vec.push_back(std::move(item2));
        districts = std::make_unique<DistrictsWrapperSTL>(districts_vec);

        // Neighbor setup (D1 and D2 touch or are set up as neighbors)
        // Let's create D1 (index 0) and D2 (index 1) for neighbor tests
        DistrictItem nd1;
        nd1.first = DistrictInfo(0, "D1", "D1");
        nd1.second = TgGeomSharedPtr(
            TgGeomPtr(tg_parse_wkt("POLYGON((0 0, 1 0, 1 1, 0 1, 0 0))")));

        DistrictItem nd2;
        nd2.first = DistrictInfo(1, "D2", "D2");
        nd2.second = TgGeomSharedPtr(
            TgGeomPtr(tg_parse_wkt("POLYGON((1 0, 2 0, 2 1, 1 1, 1 0))")));

        std::vector<DistrictItem> neighbor_dists_vec;
        neighbor_dists_vec.push_back(std::move(nd1));
        neighbor_dists_vec.push_back(std::move(nd2));
        neighbor_districts =
            std::make_unique<DistrictsWrapperSTL>(neighbor_dists_vec);

        std::vector<DistrictNeighborInfo> neighbors_vec;
        neighbors_vec.push_back({0, {1}});
        neighbors_vec.push_back({1, {0}});
        neighbors =
            std::make_unique<DistrictNeighborsWrapperSTL>(neighbors_vec);

        finder_linear = DistrictFinder::create("linear");
        finder_neighbor = DistrictFinder::create("neighbors");
    }

    std::unique_ptr<DistrictsWrapperSTL> districts;
    std::unique_ptr<DistrictsWrapperSTL> neighbor_districts;
    std::unique_ptr<DistrictNeighborsWrapperSTL> neighbors;
    TgGeomPtr moscow_district_geom = nullptr;
    TgGeomPtr tver_district_geom = nullptr;
    Ptr<DistrictFinder> finder_linear = nullptr;
    Ptr<DistrictFinder> finder_neighbor = nullptr;
};

// --- Linear Finder Tests ---

TEST_F(FindDistrictTest, Linear_InsideD1_ReturnsExact) {
    Coordinates location(56.0, 36.0);
    DistrictInfo result;
    LocationInfo dummy_location{};
    auto state = finder_linear->find_district(*districts, std::nullopt,
                                              location, dummy_location, result);
    EXPECT_EQ(state, PrecisionState::Exact);
}

TEST_F(FindDistrictTest, Linear_InsideD1_CorrectId) {
    Coordinates location(56.0, 36.0);
    DistrictInfo result;
    LocationInfo dummy_location{};
    finder_linear->find_district(*districts, std::nullopt, location,
                                 dummy_location, result);
    EXPECT_EQ(result.id, 1u);
}

TEST_F(FindDistrictTest, Linear_InsideD1_CorrectNameEn) {
    Coordinates location(56.0, 36.0);
    DistrictInfo result;
    LocationInfo dummy_location{};
    finder_linear->find_district(*districts, std::nullopt, location,
                                 dummy_location, result);
    EXPECT_EQ(result.name_en, "Moscow Oblast");
}

TEST_F(FindDistrictTest, Linear_InsideD2_ReturnsExact) {
    Coordinates location(51.0, 41.0);
    DistrictInfo result;
    LocationInfo dummy_location{};
    auto state = finder_linear->find_district(*districts, std::nullopt,
                                              location, dummy_location, result);
    EXPECT_EQ(state, PrecisionState::Exact);
}

TEST_F(FindDistrictTest, Linear_InsideD2_CorrectId) {
    Coordinates location(51.0, 41.0);
    DistrictInfo result;
    LocationInfo dummy_location{};
    finder_linear->find_district(*districts, std::nullopt, location,
                                 dummy_location, result);
    EXPECT_EQ(result.id, 2u);
}

TEST_F(FindDistrictTest, Linear_Outside_ReturnsApproximate) {
    Coordinates location(39.0, 45.0);
    DistrictInfo result;
    LocationInfo dummy_location{};
    auto state = finder_linear->find_district(*districts, std::nullopt,
                                              location, dummy_location, result);
    EXPECT_EQ(state, PrecisionState::Approximate);
}

TEST_F(FindDistrictTest, Linear_EmptyList_ThrowsException) {
    std::vector<DistrictItem> empty_vec;
    DistrictsWrapperSTL empty_districts(empty_vec);
    Coordinates location(56.0, 36.0);
    DistrictInfo result;
    LocationInfo dummy_location{};
    EXPECT_THROW(finder_linear->find_district(empty_districts, std::nullopt,
                                              location, dummy_location, result),
                 std::runtime_error);
}

// --- Neighbor Finder Tests ---

TEST_F(FindDistrictTest, Neighbor_InsideBase_ReturnsExact) {
    Coordinates location(0.5, 0.5);  // Inside D1 (index 0)
    DistrictInfo result;
    LocationInfo nearest_location{};
    nearest_location.region_id = 0;  // Starts at D1
    auto state = finder_neighbor->find_district(
        *neighbor_districts, *neighbors, location, nearest_location, result);
    EXPECT_EQ(state, PrecisionState::Exact);
}

TEST_F(FindDistrictTest, Neighbor_InsideBase_CorrectId) {
    Coordinates location(0.5, 0.5);  // Inside D1 (index 0)
    DistrictInfo result;
    LocationInfo nearest_location{};
    nearest_location.region_id = 0;  // Starts at D1
    finder_neighbor->find_district(*neighbor_districts, *neighbors, location,
                                   nearest_location, result);
    EXPECT_EQ(result.id, 0u);
}

TEST_F(FindDistrictTest, Neighbor_InsideNeighbor_ReturnsExact) {
    Coordinates location(0.5, 1.5);  // Inside D2 (index 1)
    DistrictInfo result;
    LocationInfo nearest_location{};
    nearest_location.region_id =
        0;  // Starts at D1, should look at D2 which is neighbor
    auto state = finder_neighbor->find_district(
        *neighbor_districts, *neighbors, location, nearest_location, result);
    EXPECT_EQ(state, PrecisionState::Exact);
}

TEST_F(FindDistrictTest, Neighbor_InsideNeighbor_CorrectId) {
    Coordinates location(0.5, 1.5);  // Inside D2 (index 1)
    DistrictInfo result;
    LocationInfo nearest_location{};
    nearest_location.region_id =
        0;  // Starts at D1, should look at D2 which is neighbor
    finder_neighbor->find_district(*neighbor_districts, *neighbors, location,
                                   nearest_location, result);
    EXPECT_EQ(result.id, 1u);
}

TEST_F(FindDistrictTest, Neighbor_OutsideBoth_ReturnsApproximate) {
    Coordinates location(10.0, 10.0);
    DistrictInfo result;
    LocationInfo nearest_location{};
    nearest_location.region_id = 0;
    auto state = finder_neighbor->find_district(
        *neighbor_districts, *neighbors, location, nearest_location, result);
    EXPECT_EQ(state, PrecisionState::Approximate);
}

TEST_F(FindDistrictTest, Neighbor_EmptyDistricts_ThrowsException) {
    std::vector<DistrictItem> empty_vec;
    DistrictsWrapperSTL empty_districts(empty_vec);
    Coordinates location(0.5, 0.5);
    DistrictInfo result;
    LocationInfo nearest_location{};
    nearest_location.region_id = 0;
    EXPECT_THROW(
        finder_neighbor->find_district(empty_districts, *neighbors, location,
                                       nearest_location, result),
        std::runtime_error);
}

TEST_F(FindDistrictTest, Neighbor_EmptyNeighbors_ThrowsException) {
    std::vector<DistrictNeighborInfo> empty_nbr_vec;
    DistrictNeighborsWrapperSTL empty_nbrs(empty_nbr_vec);
    Coordinates location(0.5, 0.5);
    DistrictInfo result;
    LocationInfo nearest_location{};
    nearest_location.region_id = 0;
    EXPECT_THROW(
        finder_neighbor->find_district(*neighbor_districts, empty_nbrs,
                                       location, nearest_location, result),
        std::runtime_error);
}
