// Copyright 2025 itlab-vision

#include <gtest/gtest.h>
#include <cmath>

#include "coordinates.hpp"  // NOLINT
#include "distance.hpp"     // NOLINT

class DistanceTest : public ::testing::Test {
 protected:
    void SetUp() override {
        moscow = Coordinates(55.75, 37.62);
        st_petersburg = Coordinates(59.93, 30.36);
        tver = Coordinates(56.86, 35.20);
    }

    double ConvertToRadians(double degrees) {
        return degrees * 3.14159265359 / 180.0;
    }

    Coordinates moscow;
    Coordinates st_petersburg;
    Coordinates tver;
};

// Tests for Distance::create factory

TEST_F(DistanceTest, CreateHaversineType) {
    Ptr<Distance> distance_calculator = Distance::create("haversine");
    EXPECT_NE(distance_calculator, nullptr);
}

TEST_F(DistanceTest, CreateHaversineType_Derived) {
    Ptr<Distance> distance_calculator = Distance::create("haversine");
    EXPECT_NE(dynamic_cast<HaversineDistance *>(distance_calculator.get()),
              nullptr);
}

TEST_F(DistanceTest, CreateHaversineApproxType) {
    Ptr<Distance> distance_calculator = Distance::create("haversine_approx");
    EXPECT_NE(distance_calculator, nullptr);
}

TEST_F(DistanceTest, CreateHaversineApproxType_Derived) {
    Ptr<Distance> distance_calculator = Distance::create("haversine_approx");
    EXPECT_NE(
        dynamic_cast<HaversineApproxDistance *>(distance_calculator.get()),
        nullptr);
}

TEST_F(DistanceTest, CreateDifferentTypes) {
    Ptr<Distance> haversine_calc = Distance::create("haversine");
    Ptr<Distance> haversine_approx_calc = Distance::create("haversine_approx");
}

TEST_F(DistanceTest, CreateDifferentTypes_HaversineDerived) {
    Ptr<Distance> haversine_calc = Distance::create("haversine");
    EXPECT_NE(dynamic_cast<HaversineDistance *>(haversine_calc.get()), nullptr);
}

TEST_F(DistanceTest, CreateDifferentTypes_HaversineApproxDerived) {
    Ptr<Distance> haversine_approx_calc = Distance::create("haversine_approx");
    EXPECT_NE(
        dynamic_cast<HaversineApproxDistance *>(haversine_approx_calc.get()),
        nullptr);
}
TEST_F(DistanceTest, CreateWithUnsupportedTypeThrows) {
    EXPECT_THROW(
        { Distance::create("unsupported_formula"); }, std::runtime_error);
}

// Tests for HaversineDistance::calculate

TEST_F(DistanceTest, HaversineCalculateSamePointZeroDistance) {
    HaversineDistance calculator;
    EXPECT_DOUBLE_EQ(calculator.calculate(moscow, moscow), 0.0);
}

TEST_F(DistanceTest, HaversineCalculateKnownPoints) {
    HaversineDistance calculator;

    double moscow_lat_rad = ConvertToRadians(moscow.latitude);
    double moscow_lon_rad = ConvertToRadians(moscow.longitude);
    double spb_lat_rad = ConvertToRadians(st_petersburg.latitude);
    double spb_lon_rad = ConvertToRadians(st_petersburg.longitude);

    double lat_diff = spb_lat_rad - moscow_lat_rad;
    double lon_diff = spb_lon_rad - moscow_lon_rad;
    double haversine_term = sin(lat_diff / 2.0) * sin(lat_diff / 2.0) +
                            cos(moscow_lat_rad) * cos(spb_lat_rad) *
                                sin(lon_diff / 2.0) * sin(lon_diff / 2.0);
    double expected_distance =
        6371.0 * 2.0 * atan2(sqrt(haversine_term), sqrt(1 - haversine_term));

    double actual_distance = calculator.calculate(moscow, st_petersburg);
    EXPECT_NEAR(actual_distance, expected_distance, 1e-6);
}

// Tests for HaversineApproxDistance::calculate

TEST_F(DistanceTest, HaversineApproxCalculateSamePointZeroDistance) {
    HaversineApproxDistance calculator;
    EXPECT_DOUBLE_EQ(calculator.calculate(moscow, moscow), 0.0);
}

TEST_F(DistanceTest, HaversineApproxCalculateApproximatesExactHaversine) {
    HaversineApproxDistance approximate_calculator;
    HaversineDistance exact_calculator;

    double exact_distance = exact_calculator.calculate(moscow, tver);
    double approximate_distance =
        approximate_calculator.calculate(moscow, tver);

    EXPECT_NEAR(approximate_distance, exact_distance, 1e-1);
}
