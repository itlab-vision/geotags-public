// Copyright 2025 itlab-vision
#include <gtest/gtest.h>
#include <memory>
#include <vector>
#include <string>
#include <stdexcept>
#include <utility>
#include <optional>

#include "generic_nearest_point.hpp"  // NOLINT
#include "coordinates.hpp"            // NOLINT
#include "location_info.hpp"          // NOLINT
#include "attraction_info.hpp"        // NOLINT
#include "locations_parser.hpp"       // NOLINT
#include "attractions_parser.hpp"     // NOLINT
#include "distance.hpp"               // NOLINT
#include "points_wrapper_stl.hpp"     // NOLINT

struct LocationTestTraits {
    using InfoType = LocationInfo;
    using WrapperType = PointsWrapperSTL<LocationItem>;
    using ItemType = LocationItem;

    static ItemType make_item(const std::string& name, double lat, double lon) {
        return LocationItem{LocationInfo(0, "Russia", name, -1, -1, {"Moskva"}),
                            Coordinates(lat, lon)};
    }
    static std::string get_name(const InfoType& info) { return info.city; }
};

struct AttractionTestTraits {
    using InfoType = AttractionInfo;
    using WrapperType = PointsWrapperSTL<AttractionItem>;
    using ItemType = AttractionItem;

    static ItemType make_item(const std::string& name, double lat, double lon) {
        return AttractionItem{AttractionInfo("museum", name),
                              Coordinates(lat, lon)};
    }
    static std::string get_name(const InfoType& info) { return info.name; }
};

template <typename T>
class NearestPointLinearTest : public ::testing::Test {
 protected:
    void SetUp() override {
        dist_calculator = Distance::create("haversine");
        searcher =
            GenericNearestPoint<typename T::InfoType,
                                typename T::WrapperType>::create("linear");

        auto item1 = T::make_item("Moscow", 55.75, 37.62);
        auto item2 = T::make_item("Tver", 56.86, 35.20);
        auto item3 = T::make_item("Vladimir", 56.14, 40.41);

        std::vector<typename T::ItemType> data = {item1, item2, item3};
        points = std::make_unique<typename T::WrapperType>(data);
    }

    Ptr<Distance> dist_calculator;
    Ptr<GenericNearestPoint<typename T::InfoType, typename T::WrapperType>>
        searcher;
    std::unique_ptr<typename T::WrapperType> points;
};

using TestTypes = ::testing::Types<LocationTestTraits, AttractionTestTraits>;
TYPED_TEST_SUITE(NearestPointLinearTest, TestTypes);

TYPED_TEST(NearestPointLinearTest, Basic_ReturnsExact) {
    Coordinates query(55.80, 37.70);
    typename TypeParam::InfoType result;
    double distance;
    auto state = this->searcher->calculate(*(this->points), std::nullopt,
                                           this->dist_calculator, query, result,
                                           distance, 0.0);
    EXPECT_EQ(state, PrecisionState::Exact);
}

TYPED_TEST(NearestPointLinearTest, Basic_CorrectName) {
    Coordinates query(55.80, 37.70);
    typename TypeParam::InfoType result;
    double distance;
    this->searcher->calculate(*(this->points), std::nullopt,
                              this->dist_calculator, query, result, distance,
                              0.0);
    EXPECT_EQ(TypeParam::get_name(result), "Moscow");
}

TYPED_TEST(NearestPointLinearTest, Basic_DistanceIsPositive) {
    Coordinates query(55.80, 37.70);
    typename TypeParam::InfoType result;
    double distance;
    this->searcher->calculate(*(this->points), std::nullopt,
                              this->dist_calculator, query, result, distance,
                              0.0);
    EXPECT_GT(distance, 0.0);
}

TYPED_TEST(NearestPointLinearTest, ExactMatch_DistanceIsZero) {
    Coordinates query(55.75, 37.62);
    typename TypeParam::InfoType result;
    double distance;
    this->searcher->calculate(*(this->points), std::nullopt,
                              this->dist_calculator, query, result, distance,
                              0.0);
    EXPECT_DOUBLE_EQ(distance, 0.0);
}

TYPED_TEST(NearestPointLinearTest, ExactMatch_CorrectName) {
    Coordinates query(55.75, 37.62);
    typename TypeParam::InfoType result;
    double distance;
    this->searcher->calculate(*(this->points), std::nullopt,
                              this->dist_calculator, query, result, distance,
                              0.0);
    EXPECT_EQ(TypeParam::get_name(result), "Moscow");
}

TYPED_TEST(NearestPointLinearTest, WithThreshold_ReturnsApproximate) {
    Coordinates query(60.0, 40.0);
    typename TypeParam::InfoType result;
    double distance;
    auto state = this->searcher->calculate(*(this->points), std::nullopt,
                                           this->dist_calculator, query, result,
                                           distance, 1.0);
    EXPECT_EQ(state, PrecisionState::Approximate);
}

TYPED_TEST(NearestPointLinearTest, EmptyList_ThrowsException) {
    std::vector<typename TypeParam::ItemType> empty_vec;
    typename TypeParam::WrapperType empty_points(empty_vec);
    Coordinates query(55.75, 37.62);
    typename TypeParam::InfoType result;
    double distance;
    EXPECT_THROW(this->searcher->calculate(empty_points, std::nullopt,
                                           this->dist_calculator, query, result,
                                           distance, 0.0),
                 std::runtime_error);
}

// Location-specific test
TEST(NearestLocationLinearSpecificTest, AlternativeNames_CorrectFirst) {
    auto dist_calculator = Distance::create("haversine");
    auto searcher =
        GenericNearestPoint<LocationInfo,
                            PointsWrapperSTL<LocationItem>>::create("linear");
    auto item =
        LocationItem{LocationInfo(0, "Russia", "Moscow", -1, -1, {"Moskva"}),
                     Coordinates(55.75, 37.62)};
    std::vector<LocationItem> data = {item};
    PointsWrapperSTL<LocationItem> locations(data);

    Coordinates query(55.80, 37.70);
    LocationInfo result;
    double distance;
    searcher->calculate(locations, std::nullopt, dist_calculator, query, result,
                        distance, 0.0);
    ASSERT_GE(result.alt_names.size(), 1u);
    EXPECT_EQ(result.alt_names[0], "Moskva");
}
