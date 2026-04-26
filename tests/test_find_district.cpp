// Copyright 2025 itlab-vision

#include <gtest/gtest.h>
#include <memory>
#include <vector>
#include <string>
#include <stdexcept>

#include "find_district.hpp"  // NOLINT
#include "coordinates.hpp"    // NOLINT
#include "location_info.hpp"  // NOLINT
#include "district_info.hpp"  // NOLINT

extern "C" {
#include "tg.h"  // NOLINT
}

// Tests for findDistrict() function

class FindDistrictTest : public ::testing::Test {
 protected:
    void SetUp() override {
        // Create test polygons
        moscow_district_geom = tg_parse_wkt(
            "POLYGON((35.0 55.0, 37.0 55.0, 37.0 57.0, 35.0 57.0, 35.0 55.0))");
        if (tg_geom_error(moscow_district_geom)) {
            throw std::runtime_error("Failed to parse Moscow district WKT");
        }

        tver_district_geom = tg_parse_wkt(
            "POLYGON((40.0 50.0, 42.0 50.0, 42.0 52.0, 40.0 52.0, 40.0 50.0))");
        if (tg_geom_error(tver_district_geom)) {
            throw std::runtime_error("Failed to parse Tver district WKT");
        }

        // Initialize district data
        DistrictInfo moscow(1, "Moscow Oblast", "Московская область");
        DistrictInfo tver(2, "Tver Oblast", "Тверская область");

        districts.push_back({moscow, moscow_district_geom});
        districts.push_back({tver, tver_district_geom});
    }

    void TearDown() override {
        // Clean up geometry objects
        if (moscow_district_geom) {
            tg_geom_free(moscow_district_geom);
        }
        if (tver_district_geom) {
            tg_geom_free(tver_district_geom);
        }
    }

    std::vector<Pair<DistrictInfo, tg_geom*>> districts;
    tg_geom* moscow_district_geom = nullptr;
    tg_geom* tver_district_geom = nullptr;
};

TEST_F(FindDistrictTest, ExactLocationInside) {
    // Point inside first district (Moscow Oblast)
    Coordinates location(56.0, 36.0);
    DistrictInfo result;

    auto state = find_district(districts, location, result);
    EXPECT_EQ(state, NearestLocationState::ExactLocation);
}

TEST_F(FindDistrictTest, ExactLocationInside_NameEn) {
    Coordinates location(56.0, 36.0);
    DistrictInfo result;
    find_district(districts, location, result);
    EXPECT_EQ(result.name_en, "Moscow Oblast");
}

TEST_F(FindDistrictTest, ExactLocationInside_NameLocal) {
    Coordinates location(56.0, 36.0);
    DistrictInfo result;
    find_district(districts, location, result);
    EXPECT_EQ(result.name, "Московская область");
}

TEST_F(FindDistrictTest, ExactLocationInside_Id) {
    Coordinates location(56.0, 36.0);
    DistrictInfo result;
    find_district(districts, location, result);
    EXPECT_EQ(result.id, 1);
}

TEST_F(FindDistrictTest, ExactLocationInSecondDistrict) {
    // Point inside second district (Tver Oblast)
    Coordinates location(51.0, 41.0);
    DistrictInfo result;

    auto state = find_district(districts, location, result);
    EXPECT_EQ(state, NearestLocationState::ExactLocation);
}

TEST_F(FindDistrictTest, ExactLocationInSecondDistrict_NameEn) {
    Coordinates location(51.0, 41.0);
    DistrictInfo result;
    find_district(districts, location, result);
    EXPECT_EQ(result.name_en, "Tver Oblast");
}

TEST_F(FindDistrictTest, ExactLocationInSecondDistrict_NameLocal) {
    Coordinates location(51.0, 41.0);
    DistrictInfo result;
    find_district(districts, location, result);
    EXPECT_EQ(result.name, "Тверская область");
}

TEST_F(FindDistrictTest, ExactLocationInSecondDistrict_Id) {
    Coordinates location(51.0, 41.0);
    DistrictInfo result;
    find_district(districts, location, result);
    EXPECT_EQ(result.id, 2);
}

TEST_F(FindDistrictTest, ApproximateLocationOutside) {
    // Point outside all districts
    Coordinates location(39.0, 45.0);
    DistrictInfo result;

    auto state = find_district(districts, location, result);

    EXPECT_EQ(state, NearestLocationState::ApproximateLocation);
}

TEST_F(FindDistrictTest, BoundaryPoint) {
    // Point on the boundary of Moscow Oblast
    Coordinates location(55.0, 35.0);
    DistrictInfo result;

    auto state = find_district(districts, location, result);

    // Boundary behavior depends on tg_geom_contains implementation
    EXPECT_TRUE(state == NearestLocationState::ExactLocation ||
                state == NearestLocationState::ApproximateLocation);
}

TEST_F(FindDistrictTest, EmptyList) {
    std::vector<Pair<DistrictInfo, tg_geom*>> empty_districts;
    Coordinates location(56.0, 36.0);
    DistrictInfo result;

    EXPECT_THROW(
        { find_district(empty_districts, location, result); },
        std::runtime_error);
}
