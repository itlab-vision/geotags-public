// Copyright 2025 itlab-vision
#include <gtest/gtest.h>

#include "distance.hpp"     // NOLINT
#include "coordinates.hpp"  // NOLINT

class DistanceTest : public ::testing::Test {};

TEST_F(DistanceTest, Factory_Haversine) {
    auto dist = Distance::create("haversine");
    EXPECT_NE(dist, nullptr);
}

TEST_F(DistanceTest, Factory_HaversineApprox) {
    auto dist = Distance::create("haversine_approx");
    EXPECT_NE(dist, nullptr);
}

TEST_F(DistanceTest, Factory_Unknown_Throws) {
    EXPECT_THROW(Distance::create("unknown_formula"), std::runtime_error);
}

TEST_F(DistanceTest, Haversine_ZeroDistance) {
    auto dist = Distance::create("haversine");
    double res = dist->calculate(55.7558, 37.6173, 55.7558, 37.6173);
    EXPECT_NEAR(res, 0.0, 1e-6);
}

TEST_F(DistanceTest, HaversineApprox_ZeroDistance) {
    auto dist = Distance::create("haversine_approx");
    double res = dist->calculate(55.7558, 37.6173, 55.7558, 37.6173);
    EXPECT_NEAR(res, 0.0, 1e-6);
}

TEST_F(DistanceTest, Haversine_KnownDistance_Moscow_SPB) {
    auto dist = Distance::create("haversine");
    // Moscow (55.7558, 37.6173) to St. Petersburg (59.9343, 30.3351)
    // Real distance is ~634 km
    double res = dist->calculate(55.7558, 37.6173, 59.9343, 30.3351);
    EXPECT_NEAR(res, 634.0, 5.0);
}

TEST_F(DistanceTest, Haversine_vs_Approx_SmallDistance) {
    auto dist_exact = Distance::create("haversine");
    auto dist_approx = Distance::create("haversine_approx");

    // Moscow center (55.7558, 37.6173) to Mytishchi (55.9105, 37.7322) - ~18 km
    double lat1 = 55.7558, lon1 = 37.6173;
    double lat2 = 55.9105, lon2 = 37.7322;

    double res_exact = dist_exact->calculate(lat1, lon1, lat2, lon2);
    double res_approx = dist_approx->calculate(lat1, lon1, lat2, lon2);

    // On small distances, approx should be very close to exact
    EXPECT_NEAR(res_exact, res_approx, 0.5);
}

TEST_F(DistanceTest, Coordinates_DefaultConstructor_Latitude) {
    Coordinates c;
    EXPECT_DOUBLE_EQ(c.latitude, 0.0);
}

TEST_F(DistanceTest, Coordinates_DefaultConstructor_Longitude) {
    Coordinates c;
    EXPECT_DOUBLE_EQ(c.longitude, 0.0);
}

TEST_F(DistanceTest, Coordinates_ParamConstructor_Latitude) {
    Coordinates c(12.3, 45.6);
    EXPECT_DOUBLE_EQ(c.latitude, 12.3);
}

TEST_F(DistanceTest, Coordinates_ParamConstructor_Longitude) {
    Coordinates c(12.3, 45.6);
    EXPECT_DOUBLE_EQ(c.longitude, 45.6);
}
