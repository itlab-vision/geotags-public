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
#include "segments_wrapper_stl.hpp"   // NOLINT

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
class NearestPointGridTest : public ::testing::Test {
 protected:
    void SetUp() override {
        dist_calculator = Distance::create("haversine");
        searcher_grid =
            GenericNearestPoint<typename T::InfoType,
                                typename T::WrapperType>::create("grid");
        searcher_grid_binary =
            GenericNearestPoint<typename T::InfoType,
                                typename T::WrapperType>::create("grid_binary");

        auto item1 = T::make_item("Moscow", 55.75, 37.62);
        auto item2 = T::make_item("Tver", 56.86, 35.20);
        auto item3 = T::make_item("Vladimir", 56.14, 40.41);

        std::vector<typename T::ItemType> data = {item1, item2, item3};
        points = std::make_unique<typename T::WrapperType>(data);

        // Build 2x2 grid:
        // Cell 0: lat [55.0, 57.0], lon [30.0, 36.0] -> points: Tver (idx 1)
        // Cell 1: lat [55.0, 57.0], lon [36.0, 42.0] -> points: Moscow (idx 0),
        // Vladimir (idx 2) Cell 2: lat [57.0, 59.0], lon [30.0, 36.0] ->
        // points: empty Cell 3: lat [57.0, 59.0], lon [36.0, 42.0] -> points:
        // empty
        std::vector<SegmentInfo> segs;
        segs.push_back(
            SegmentInfo(0, {55.0, 57.0}, {30.0, 36.0}, {1, 2, 3}, {1}));
        segs.push_back(
            SegmentInfo(1, {55.0, 57.0}, {36.0, 42.0}, {0, 2, 3}, {0, 2}));
        segs.push_back(
            SegmentInfo(2, {57.0, 59.0}, {30.0, 36.0}, {0, 1, 3}, {}));
        segs.push_back(
            SegmentInfo(3, {57.0, 59.0}, {36.0, 42.0}, {0, 1, 2}, {}));

        segments = std::make_unique<SegmentsWrapperSTL>(segs);
    }

    Ptr<Distance> dist_calculator;
    Ptr<GenericNearestPoint<typename T::InfoType, typename T::WrapperType>>
        searcher_grid;
    Ptr<GenericNearestPoint<typename T::InfoType, typename T::WrapperType>>
        searcher_grid_binary;
    std::unique_ptr<typename T::WrapperType> points;
    std::unique_ptr<SegmentsWrapperSTL> segments;
};

using TestTypes = ::testing::Types<LocationTestTraits, AttractionTestTraits>;
TYPED_TEST_SUITE(NearestPointGridTest, TestTypes);

// --- Grid tests ---

TYPED_TEST(NearestPointGridTest, Grid_Basic_ReturnsExact) {
    Coordinates query(55.80, 37.70);
    typename TypeParam::InfoType result;
    double distance;
    auto state = this->searcher_grid->calculate(
        *(this->points), *(this->segments), this->dist_calculator, query,
        result, distance, 0.0);
    EXPECT_EQ(state, PrecisionState::Exact);
}

TYPED_TEST(NearestPointGridTest, Grid_Basic_CorrectName) {
    Coordinates query(55.80, 37.70);
    typename TypeParam::InfoType result;
    double distance;
    this->searcher_grid->calculate(*(this->points), *(this->segments),
                                   this->dist_calculator, query, result,
                                   distance, 0.0);
    EXPECT_EQ(TypeParam::get_name(result), "Moscow");
}

TYPED_TEST(NearestPointGridTest, Grid_Basic_DistanceIsPositive) {
    Coordinates query(55.80, 37.70);
    typename TypeParam::InfoType result;
    double distance;
    this->searcher_grid->calculate(*(this->points), *(this->segments),
                                   this->dist_calculator, query, result,
                                   distance, 0.0);
    EXPECT_GT(distance, 0.0);
}

TYPED_TEST(NearestPointGridTest, Grid_ExactMatch_DistanceIsZero) {
    Coordinates query(55.75, 37.62);
    typename TypeParam::InfoType result;
    double distance;
    this->searcher_grid->calculate(*(this->points), *(this->segments),
                                   this->dist_calculator, query, result,
                                   distance, 0.0);
    EXPECT_DOUBLE_EQ(distance, 0.0);
}

TYPED_TEST(NearestPointGridTest, Grid_WithThreshold_ReturnsApproximate) {
    Coordinates query(60.0, 40.0);
    typename TypeParam::InfoType result;
    double distance;
    auto state = this->searcher_grid->calculate(
        *(this->points), *(this->segments), this->dist_calculator, query,
        result, distance, 1.0);
    EXPECT_EQ(state, PrecisionState::Approximate);
}

// --- Grid Binary tests ---

TYPED_TEST(NearestPointGridTest, GridBinary_Basic_ReturnsExact) {
    Coordinates query(55.80, 37.70);
    typename TypeParam::InfoType result;
    double distance;
    auto state = this->searcher_grid_binary->calculate(
        *(this->points), *(this->segments), this->dist_calculator, query,
        result, distance, 0.0);
    EXPECT_EQ(state, PrecisionState::Exact);
}

TYPED_TEST(NearestPointGridTest, GridBinary_Basic_CorrectName) {
    Coordinates query(55.80, 37.70);
    typename TypeParam::InfoType result;
    double distance;
    this->searcher_grid_binary->calculate(*(this->points), *(this->segments),
                                          this->dist_calculator, query, result,
                                          distance, 0.0);
    EXPECT_EQ(TypeParam::get_name(result), "Moscow");
}

TYPED_TEST(NearestPointGridTest, GridBinary_Basic_DistanceIsPositive) {
    Coordinates query(55.80, 37.70);
    typename TypeParam::InfoType result;
    double distance;
    this->searcher_grid_binary->calculate(*(this->points), *(this->segments),
                                          this->dist_calculator, query, result,
                                          distance, 0.0);
    EXPECT_GT(distance, 0.0);
}

TYPED_TEST(NearestPointGridTest, GridBinary_ExactMatch_DistanceIsZero) {
    Coordinates query(55.75, 37.62);
    typename TypeParam::InfoType result;
    double distance;
    this->searcher_grid_binary->calculate(*(this->points), *(this->segments),
                                          this->dist_calculator, query, result,
                                          distance, 0.0);
    EXPECT_DOUBLE_EQ(distance, 0.0);
}

TYPED_TEST(NearestPointGridTest, GridBinary_WithThreshold_ReturnsApproximate) {
    Coordinates query(60.0, 40.0);
    typename TypeParam::InfoType result;
    double distance;
    auto state = this->searcher_grid_binary->calculate(
        *(this->points), *(this->segments), this->dist_calculator, query,
        result, distance, 1.0);
    EXPECT_EQ(state, PrecisionState::Approximate);
}

// Common error tests
TYPED_TEST(NearestPointGridTest, EmptyList_ThrowsException) {
    std::vector<typename TypeParam::ItemType> empty_vec;
    typename TypeParam::WrapperType empty_points(empty_vec);
    Coordinates query(55.75, 37.62);
    typename TypeParam::InfoType result;
    double distance;
    EXPECT_THROW(this->searcher_grid->calculate(empty_points, *(this->segments),
                                                this->dist_calculator, query,
                                                result, distance, 0.0),
                 std::runtime_error);
}

// Location-specific test
TEST(NearestLocationGridSpecificTest, AlternativeNames_CorrectFirst) {
    auto dist_calculator = Distance::create("haversine");
    auto searcher =
        GenericNearestPoint<LocationInfo,
                            PointsWrapperSTL<LocationItem>>::create("grid");
    auto item =
        LocationItem{LocationInfo(0, "Russia", "Moscow", -1, -1, {"Moskva"}),
                     Coordinates(55.75, 37.62)};
    std::vector<LocationItem> data = {item};
    PointsWrapperSTL<LocationItem> locations(data);

    std::vector<SegmentInfo> segs;
    segs.push_back(SegmentInfo(0, {55.0, 57.0}, {36.0, 42.0}, {}, {0}));
    SegmentsWrapperSTL segments(segs);

    Coordinates query(55.80, 37.70);
    LocationInfo result;
    double distance;
    searcher->calculate(locations, segments, dist_calculator, query, result,
                        distance, 0.0);
    ASSERT_GE(result.alt_names.size(), 1u);
    EXPECT_EQ(result.alt_names[0], "Moskva");
}
