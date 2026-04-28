// Copyright 2025 itlab-vision

#include <flatbuffers/flatbuffers.h>
#include <gtest/gtest.h>
#include <cmath>

#include "coordinates.hpp"          // NOLINT
#include "distance.hpp"             // NOLINT
#include "locations_generated.hpp"  // NOLINT

class DistanceFBTest : public ::testing::Test {
 protected:
    void SetUp() override {
        LocationsData::Coordinates coords_local(55.75, 37.62);
        auto info_off = LocationsData::CreateLocationInfoDirect(
            builder, 0, "Russia", "Moscow", -1, -1, nullptr);
        auto loc_off =
            LocationsData::CreateLocation(builder, info_off, &coords_local);
        auto vec_off = builder.CreateVector(&loc_off, 1);
        auto root_off = LocationsData::CreateLocations(builder, vec_off);
        LocationsData::FinishLocationsBuffer(builder, root_off);

        auto root = LocationsData::GetLocations(builder.GetBufferPointer());
        fb_coords = root->data()->Get(0)->coords();
        plain_coords =
            Coordinates(fb_coords->latitude(), fb_coords->longitude());
    }

    flatbuffers::FlatBufferBuilder builder;
    const LocationsData::Coordinates *fb_coords = nullptr;
    Coordinates plain_coords;

    double ConvertToRadians(double degrees) {
        return degrees * 3.14159265359 / 180.0;
    }
};

// Tests for Distance::create factory

TEST_F(DistanceFBTest, CreateHaversineTypeFB) {
    Ptr<Distance> distance_calculator = Distance::create("haversine");
    EXPECT_NE(distance_calculator, nullptr);
}

TEST_F(DistanceFBTest, CreateHaversineTypeFB_Derived) {
    Ptr<Distance> distance_calculator = Distance::create("haversine");
    EXPECT_NE(dynamic_cast<HaversineDistance *>(distance_calculator.get()),
              nullptr);
}

TEST_F(DistanceFBTest, CreateHaversineApproxTypeFB) {
    Ptr<Distance> distance_calculator = Distance::create("haversine_approx");
    EXPECT_NE(distance_calculator, nullptr);
}

TEST_F(DistanceFBTest, CreateHaversineApproxTypeFB_Derived) {
    Ptr<Distance> distance_calculator = Distance::create("haversine_approx");
    EXPECT_NE(
        dynamic_cast<HaversineApproxDistance *>(distance_calculator.get()),
        nullptr);
}

TEST_F(DistanceFBTest, CreateDifferentTypesFB) {
    Ptr<Distance> haversine_calc = Distance::create("haversine");
    Ptr<Distance> haversine_approx_calc = Distance::create("haversine_approx");
}

TEST_F(DistanceFBTest, CreateDifferentTypesFB_HaversineDerived) {
    Ptr<Distance> haversine_calc = Distance::create("haversine");
    EXPECT_NE(dynamic_cast<HaversineDistance *>(haversine_calc.get()), nullptr);
}

TEST_F(DistanceFBTest, CreateDifferentTypesFB_HaversineApproxDerived) {
    Ptr<Distance> haversine_approx_calc = Distance::create("haversine_approx");
    EXPECT_NE(
        dynamic_cast<HaversineApproxDistance *>(haversine_approx_calc.get()),
        nullptr);
}

TEST_F(DistanceFBTest, CreateWithUnsupportedTypeThrowsFB) {
    EXPECT_THROW(
        { Distance::create("unsupported_formula"); }, std::runtime_error);
}

// Tests for HaversineDistance::calculate

TEST_F(DistanceFBTest, HaversineCalculateSamePointZeroDistanceFB) {
    HaversineDistance calculator;
    double result = calculator.calculate(fb_coords, plain_coords);
    EXPECT_NEAR(result, 0.0, 1e-12);
}

TEST_F(DistanceFBTest, HaversineCalculateKnownPointsFB) {
    HaversineDistance calculator;
    LocationsData::Coordinates spb_fb(59.93, 30.36);

    double lat1 = ConvertToRadians(fb_coords->latitude());
    double lon1 = ConvertToRadians(fb_coords->longitude());
    double lat2 = ConvertToRadians(spb_fb.latitude());
    double lon2 = ConvertToRadians(spb_fb.longitude());
    double dlat = lat2 - lat1;
    double dlon = lon2 - lon1;
    double tmp = sin(dlat / 2.0) * sin(dlat / 2.0) +
                 cos(lat1) * cos(lat2) * sin(dlon / 2.0) * sin(dlon / 2.0);
    double expected = 6371.0 * 2.0 * atan2(sqrt(tmp), sqrt(1 - tmp));

    double actual = calculator.calculate(
        fb_coords, Coordinates(spb_fb.latitude(), spb_fb.longitude()));
    EXPECT_NEAR(actual, expected, 1e-6);
}

// Tests for HaversineApproxDistance::calculate

TEST_F(DistanceFBTest, HaversineApproxCalculateSamePointZeroDistanceFB) {
    HaversineApproxDistance calculator;
    double result = calculator.calculate(fb_coords, plain_coords);
    EXPECT_NEAR(result, 0.0, 1e-12);
}

TEST_F(DistanceFBTest, HaversineApproxApproximatesExactFB) {
    HaversineApproxDistance approx;
    HaversineDistance exact;
    Coordinates target(56.86, 35.20);

    double exact_d = exact.calculate(fb_coords, target);
    double approx_d = approx.calculate(fb_coords, target);
    EXPECT_NEAR(approx_d, exact_d, 1e-1);
}
