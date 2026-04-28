// Copyright 2025 itlab-vision

#include <gtest/gtest.h>
#include <memory>
#include <vector>
#include <string>
#include <stdexcept>
#include <utility>

#include "nearest_location.hpp"  // NOLINT
#include "coordinates.hpp"       // NOLINT
#include "location_info.hpp"     // NOLINT
#include "district_info.hpp"     // NOLINT
#include "distance.hpp"          // NOLINT

extern "C" {
#include "tg.h"  // NOLINT
}

class NearestLocationLinearTest : public ::testing::Test {
 protected:
    void SetUp() override {
        dist_calculator = Distance::create("haversine");
        if (!dist_calculator) {
            throw std::runtime_error("Failed to create distance calculator");
        }

        searcher = NearestLocation::create("linear");
        if (!searcher) {
            throw std::runtime_error("Failed to create NearestLocationLinear");
        }

        moscow = LocationInfo("Russia", "Moscow", {"Moskva"});
        tver = LocationInfo("Russia", "Tver", {"Tver City"});
        vladimir = LocationInfo("Russia", "Vladimir", {"Vladimir City"});
        spb = LocationInfo("Russia", "St. Petersburg", {"SPB", "Piter"});
        novgorod = LocationInfo("Russia", "Novgorod", {"Great Novgorod"});

        moscow_coords = Coordinates(55.75, 37.62);
        tver_coords = Coordinates(56.86, 35.20);
        vladimir_coords = Coordinates(56.14, 40.41);
        spb_coords = Coordinates(59.93, 30.36);
        novgorod_coords = Coordinates(58.52, 31.28);
    }

    std::vector<Pair<LocationInfo, Coordinates>> CreateLocationList(
        const std::vector<std::pair<LocationInfo, Coordinates>>& data) {
        std::vector<Pair<LocationInfo, Coordinates>> result;
        for (const auto& pair : data) {
            result.push_back(pair);
        }
        return result;
    }

    Ptr<Distance> dist_calculator;
    Ptr<NearestLocation> searcher;
    LocationInfo moscow, tver, vladimir, spb, novgorod;
    Coordinates moscow_coords, tver_coords, vladimir_coords, spb_coords,
        novgorod_coords;
};

TEST_F(NearestLocationLinearTest, Basic) {
    auto locations = CreateLocationList({{moscow, moscow_coords},
                                         {tver, tver_coords},
                                         {vladimir, vladimir_coords}});

    Coordinates query(55.80, 37.70);
    LocationInfo result;
    double distance;

    auto state = searcher->calculate(locations, {}, dist_calculator, query,
                                     result, distance, 0.0);

    EXPECT_EQ(state, NearestLocationState::ExactLocation);
}

TEST_F(NearestLocationLinearTest, Basic_City) {
    auto locations = CreateLocationList({{moscow, moscow_coords},
                                         {tver, tver_coords},
                                         {vladimir, vladimir_coords}});
    Coordinates query(55.80, 37.70);
    LocationInfo result;
    double distance;
    searcher->calculate(locations, {}, dist_calculator, query, result, distance,
                        0.0);
    EXPECT_EQ(result.city, "Moscow");
}

TEST_F(NearestLocationLinearTest, Basic_Country) {
    auto locations = CreateLocationList({{moscow, moscow_coords},
                                         {tver, tver_coords},
                                         {vladimir, vladimir_coords}});
    Coordinates query(55.80, 37.70);
    LocationInfo result;
    double distance;
    searcher->calculate(locations, {}, dist_calculator, query, result, distance,
                        0.0);
    EXPECT_EQ(result.country, "Russia");
}

TEST_F(NearestLocationLinearTest, Basic_DistancePositive) {
    auto locations = CreateLocationList({{moscow, moscow_coords},
                                         {tver, tver_coords},
                                         {vladimir, vladimir_coords}});
    Coordinates query(55.80, 37.70);
    LocationInfo result;
    double distance;
    searcher->calculate(locations, {}, dist_calculator, query, result, distance,
                        0.0);
    EXPECT_GT(distance, 0.0);
}

TEST_F(NearestLocationLinearTest, ExactMatch) {
    auto locations = CreateLocationList({{moscow, moscow_coords}});

    Coordinates query(55.75, 37.62);
    LocationInfo result;
    double distance;

    auto state = searcher->calculate(locations, {}, dist_calculator, query,
                                     result, distance, 0.0);

    EXPECT_EQ(state, NearestLocationState::ExactLocation);
}

TEST_F(NearestLocationLinearTest, ExactMatch_City) {
    auto locations = CreateLocationList({{moscow, moscow_coords}});
    Coordinates query(55.75, 37.62);
    LocationInfo result;
    double distance;
    searcher->calculate(locations, {}, dist_calculator, query, result, distance,
                        0.0);
    EXPECT_EQ(result.city, "Moscow");
}

TEST_F(NearestLocationLinearTest, ExactMatch_DistanceZero) {
    auto locations = CreateLocationList({{moscow, moscow_coords}});
    Coordinates query(55.75, 37.62);
    LocationInfo result;
    double distance;
    searcher->calculate(locations, {}, dist_calculator, query, result, distance,
                        0.0);
    EXPECT_DOUBLE_EQ(distance, 0.0);
}

TEST_F(NearestLocationLinearTest, MultipleLocations) {
    auto locations = CreateLocationList({{moscow, moscow_coords},
                                         {spb, spb_coords},
                                         {novgorod, novgorod_coords},
                                         {tver, tver_coords}});

    Coordinates query(56.80, 35.15);
    LocationInfo result;
    double distance;

    auto state = searcher->calculate(locations, {}, dist_calculator, query,
                                     result, distance, 0.0);

    EXPECT_EQ(state, NearestLocationState::ExactLocation);
}

TEST_F(NearestLocationLinearTest, MultipleLocations_City) {
    auto locations = CreateLocationList({{moscow, moscow_coords},
                                         {spb, spb_coords},
                                         {novgorod, novgorod_coords},
                                         {tver, tver_coords}});
    Coordinates query(56.80, 35.15);
    LocationInfo result;
    double distance;
    searcher->calculate(locations, {}, dist_calculator, query, result, distance,
                        0.0);
    EXPECT_EQ(result.city, "Tver");
}

TEST_F(NearestLocationLinearTest, MultipleLocations_DistancePositive) {
    auto locations = CreateLocationList({{moscow, moscow_coords},
                                         {spb, spb_coords},
                                         {novgorod, novgorod_coords},
                                         {tver, tver_coords}});
    Coordinates query(56.80, 35.15);
    LocationInfo result;
    double distance;
    searcher->calculate(locations, {}, dist_calculator, query, result, distance,
                        0.0);
    EXPECT_GT(distance, 0.0);
}

TEST_F(NearestLocationLinearTest, WithThreshold) {
    auto locations =
        CreateLocationList({{moscow, moscow_coords}, {tver, tver_coords}});

    Coordinates query(60.0, 40.0);
    LocationInfo result;
    double distance;

    auto state = searcher->calculate(locations, {}, dist_calculator, query,
                                     result, distance, 1.0);

    EXPECT_EQ(state, NearestLocationState::ApproximateLocation);
}

TEST_F(NearestLocationLinearTest, WithThreshold_Distance) {
    auto locations =
        CreateLocationList({{moscow, moscow_coords}, {tver, tver_coords}});
    Coordinates query(60.0, 40.0);
    LocationInfo result;
    double distance;
    searcher->calculate(locations, {}, dist_calculator, query, result, distance,
                        1.0);
    EXPECT_GT(distance, 1.0);
}

TEST_F(NearestLocationLinearTest, ThresholdZero) {
    auto locations = CreateLocationList({{moscow, moscow_coords}});

    Coordinates query(55.75, 37.62);
    LocationInfo result;
    double distance;

    auto state = searcher->calculate(locations, {}, dist_calculator, query,
                                     result, distance, 0.0);

    EXPECT_EQ(state, NearestLocationState::ExactLocation);
}

TEST_F(NearestLocationLinearTest, ThresholdZero_DistanceZero) {
    auto locations = CreateLocationList({{moscow, moscow_coords}});
    Coordinates query(55.75, 37.62);
    LocationInfo result;
    double distance;
    searcher->calculate(locations, {}, dist_calculator, query, result, distance,
                        0.0);
    EXPECT_DOUBLE_EQ(distance, 0.0);
}

TEST_F(NearestLocationLinearTest, AlternativeNames) {
    auto locations =
        CreateLocationList({{moscow, moscow_coords}, {spb, spb_coords}});

    Coordinates query(55.80, 37.70);
    LocationInfo result;
    double distance;

    auto state = searcher->calculate(locations, {}, dist_calculator, query,
                                     result, distance, 0.0);

    EXPECT_EQ(result.city, "Moscow");
}

TEST_F(NearestLocationLinearTest, AlternativeNames_Size) {
    auto locations =
        CreateLocationList({{moscow, moscow_coords}, {spb, spb_coords}});
    Coordinates query(55.80, 37.70);
    LocationInfo result;
    double distance;
    searcher->calculate(locations, {}, dist_calculator, query, result, distance,
                        0.0);
    EXPECT_EQ(result.alt_names.size(), 1);
}

TEST_F(NearestLocationLinearTest, AlternativeNames_FirstAlt) {
    auto locations =
        CreateLocationList({{moscow, moscow_coords}, {spb, spb_coords}});
    Coordinates query(55.80, 37.70);
    LocationInfo result;
    double distance;
    searcher->calculate(locations, {}, dist_calculator, query, result, distance,
                        0.0);
    EXPECT_EQ(result.alt_names[0], "Moskva");
}

TEST_F(NearestLocationLinearTest, EmptyList) {
    std::vector<Pair<LocationInfo, Coordinates>> empty_locations;

    Coordinates query(55.75, 37.62);
    LocationInfo result;
    double distance;

    EXPECT_THROW(
        {
            searcher->calculate(empty_locations, {}, dist_calculator, query,
                                result, distance, 0.0);
        },
        std::runtime_error);
}
