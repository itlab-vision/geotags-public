// Copyright 2025 itlab-vision
#include <gtest/gtest.h>
#include <cstring>
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
#include "flatbuffers/flatbuffers.h"  // NOLINT
#include "points_wrapper_fb.hpp"      // NOLINT
#include "segments_wrapper_fb.hpp"    // NOLINT
#include "locations_converter.hpp"    // NOLINT
#include "attractions_converter.hpp"  // NOLINT
#include "segments_converter.hpp"     // NOLINT

struct LocationTestTraitsFB {
    using InfoType = LocationInfo;
    using FBType = LocationsData::Location;
    using WrapperType = PointsWrapperFB<FBType>;
    using ItemType = LocationItem;

    static ItemType make_item(const std::string& name, double lat, double lon) {
        return LocationItem{LocationInfo(0, "Russia", name, -1, -1, {"Moskva"}),
                            Coordinates(lat, lon)};
    }
    static std::string get_name(const InfoType& info) { return info.city; }
    static std::unique_ptr<WrapperType> make_wrapper(
        const std::vector<ItemType>& items) {
        LocationsConverter converter{};
        flatbuffers::FlatBufferBuilder builder;
        std::vector<flatbuffers::Offset<FBType>> offsets;
        for (const auto& item : items) {
            offsets.push_back(converter.convert_to_fb(builder, item));
        }
        auto root =
            converter.create_root_fb(builder, builder.CreateVector(offsets));
        builder.Finish(root);

        const uint8_t* buf = builder.GetBufferPointer();
        size_t size = builder.GetSize();
        std::shared_ptr<char> buffer(new char[size],
                                     std::default_delete<char[]>());
        std::memcpy(buffer.get(), buf, size);
        const auto* root_fb = LocationsData::GetLocations(buffer.get());
        return std::make_unique<WrapperType>(buffer, root_fb->data());
    }
};

struct AttractionTestTraitsFB {
    using InfoType = AttractionInfo;
    using FBType = AttractionsData::Attraction;
    using WrapperType = PointsWrapperFB<FBType>;
    using ItemType = AttractionItem;

    static ItemType make_item(const std::string& name, double lat, double lon) {
        return AttractionItem{AttractionInfo("museum", name),
                              Coordinates(lat, lon)};
    }
    static std::string get_name(const InfoType& info) { return info.name; }
    static std::unique_ptr<WrapperType> make_wrapper(
        const std::vector<ItemType>& items) {
        AttractionsConverter converter{};
        flatbuffers::FlatBufferBuilder builder;
        std::vector<flatbuffers::Offset<FBType>> offsets;
        for (const auto& item : items) {
            offsets.push_back(converter.convert_to_fb(builder, item));
        }
        auto root =
            converter.create_root_fb(builder, builder.CreateVector(offsets));
        builder.Finish(root);

        const uint8_t* buf = builder.GetBufferPointer();
        size_t size = builder.GetSize();
        std::shared_ptr<char> buffer(new char[size],
                                     std::default_delete<char[]>());
        std::memcpy(buffer.get(), buf, size);
        const auto* root_fb = AttractionsData::GetAttractions(buffer.get());
        return std::make_unique<WrapperType>(buffer, root_fb->data());
    }
};

template <typename T>
class NearestPointGridFBTest : public ::testing::Test {
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
        points = T::make_wrapper(data);

        // Build 2x2 grid:
        // Cell 0: lat [55.0, 57.0], lon [30.0, 36.0] -> points: Tver (idx 1)
        // Cell 1: lat [55.0, 57.0], lon [36.0, 42.0] -> points: Moscow (idx 0),
        // Vladimir (idx 2) Cell 2: lat [57.0, 59.0], lon [30.0, 36.0] ->
        // points: empty Cell 3: lat [57.0, 59.0], lon [36.0, 42.0] -> points:
        // empty
        SegmentsConverter seg_conv{};
        flatbuffers::FlatBufferBuilder seg_builder;

        SegmentInfo seg0(0, {55.0, 57.0}, {30.0, 36.0}, {1, 2, 3}, {1});
        SegmentInfo seg1(1, {55.0, 57.0}, {36.0, 42.0}, {0, 2, 3}, {0, 2});
        SegmentInfo seg2(2, {57.0, 59.0}, {30.0, 36.0}, {0, 1, 3}, {});
        SegmentInfo seg3(3, {57.0, 59.0}, {36.0, 42.0}, {0, 1, 2}, {});

        auto seg_offset0 = seg_conv.convert_to_fb(seg_builder, seg0);
        auto seg_offset1 = seg_conv.convert_to_fb(seg_builder, seg1);
        auto seg_offset2 = seg_conv.convert_to_fb(seg_builder, seg2);
        auto seg_offset3 = seg_conv.convert_to_fb(seg_builder, seg3);

        std::vector<flatbuffers::Offset<SegmentsData::SegmentInfo>> seg_vec = {
            seg_offset0, seg_offset1, seg_offset2, seg_offset3};
        auto seg_root = seg_conv.create_root_fb(
            seg_builder, seg_builder.CreateVector(seg_vec));
        seg_builder.Finish(seg_root);

        const uint8_t* seg_buf = seg_builder.GetBufferPointer();
        size_t seg_size = seg_builder.GetSize();
        std::shared_ptr<char> seg_buffer(new char[seg_size],
                                         std::default_delete<char[]>());
        std::memcpy(seg_buffer.get(), seg_buf, seg_size);
        const auto* segs_fb = SegmentsData::GetSegments(seg_buffer.get());
        segments =
            std::make_unique<SegmentsWrapperFB>(seg_buffer, segs_fb->data());
    }

    Ptr<Distance> dist_calculator;
    Ptr<GenericNearestPoint<typename T::InfoType, typename T::WrapperType>>
        searcher_grid;
    Ptr<GenericNearestPoint<typename T::InfoType, typename T::WrapperType>>
        searcher_grid_binary;
    std::unique_ptr<typename T::WrapperType> points;
    std::unique_ptr<SegmentsWrapperFB> segments;
};

using TestTypes =
    ::testing::Types<LocationTestTraitsFB, AttractionTestTraitsFB>;
TYPED_TEST_SUITE(NearestPointGridFBTest, TestTypes);

// --- Grid tests ---

TYPED_TEST(NearestPointGridFBTest, Grid_Basic_ReturnsExact) {
    Coordinates query(55.80, 37.70);
    typename TypeParam::InfoType result;
    double distance;
    auto state = this->searcher_grid->calculate(
        *(this->points), *(this->segments), this->dist_calculator, query,
        result, distance, 0.0);
    EXPECT_EQ(state, PrecisionState::Exact);
}

TYPED_TEST(NearestPointGridFBTest, Grid_Basic_CorrectName) {
    Coordinates query(55.80, 37.70);
    typename TypeParam::InfoType result;
    double distance;
    this->searcher_grid->calculate(*(this->points), *(this->segments),
                                   this->dist_calculator, query, result,
                                   distance, 0.0);
    EXPECT_EQ(TypeParam::get_name(result), "Moscow");
}

TYPED_TEST(NearestPointGridFBTest, Grid_Basic_DistanceIsPositive) {
    Coordinates query(55.80, 37.70);
    typename TypeParam::InfoType result;
    double distance;
    this->searcher_grid->calculate(*(this->points), *(this->segments),
                                   this->dist_calculator, query, result,
                                   distance, 0.0);
    EXPECT_GT(distance, 0.0);
}

TYPED_TEST(NearestPointGridFBTest, Grid_ExactMatch_DistanceIsZero) {
    Coordinates query(55.75, 37.62);
    typename TypeParam::InfoType result;
    double distance;
    this->searcher_grid->calculate(*(this->points), *(this->segments),
                                   this->dist_calculator, query, result,
                                   distance, 0.0);
    EXPECT_DOUBLE_EQ(distance, 0.0);
}

TYPED_TEST(NearestPointGridFBTest, Grid_WithThreshold_ReturnsApproximate) {
    Coordinates query(60.0, 40.0);
    typename TypeParam::InfoType result;
    double distance;
    auto state = this->searcher_grid->calculate(
        *(this->points), *(this->segments), this->dist_calculator, query,
        result, distance, 1.0);
    EXPECT_EQ(state, PrecisionState::Approximate);
}

// --- Grid Binary tests ---

TYPED_TEST(NearestPointGridFBTest, GridBinary_Basic_ReturnsExact) {
    Coordinates query(55.80, 37.70);
    typename TypeParam::InfoType result;
    double distance;
    auto state = this->searcher_grid_binary->calculate(
        *(this->points), *(this->segments), this->dist_calculator, query,
        result, distance, 0.0);
    EXPECT_EQ(state, PrecisionState::Exact);
}

TYPED_TEST(NearestPointGridFBTest, GridBinary_Basic_CorrectName) {
    Coordinates query(55.80, 37.70);
    typename TypeParam::InfoType result;
    double distance;
    this->searcher_grid_binary->calculate(*(this->points), *(this->segments),
                                          this->dist_calculator, query, result,
                                          distance, 0.0);
    EXPECT_EQ(TypeParam::get_name(result), "Moscow");
}

TYPED_TEST(NearestPointGridFBTest, GridBinary_Basic_DistanceIsPositive) {
    Coordinates query(55.80, 37.70);
    typename TypeParam::InfoType result;
    double distance;
    this->searcher_grid_binary->calculate(*(this->points), *(this->segments),
                                          this->dist_calculator, query, result,
                                          distance, 0.0);
    EXPECT_GT(distance, 0.0);
}

TYPED_TEST(NearestPointGridFBTest, GridBinary_ExactMatch_DistanceIsZero) {
    Coordinates query(55.75, 37.62);
    typename TypeParam::InfoType result;
    double distance;
    this->searcher_grid_binary->calculate(*(this->points), *(this->segments),
                                          this->dist_calculator, query, result,
                                          distance, 0.0);
    EXPECT_DOUBLE_EQ(distance, 0.0);
}

TYPED_TEST(NearestPointGridFBTest,
           GridBinary_WithThreshold_ReturnsApproximate) {
    Coordinates query(60.0, 40.0);
    typename TypeParam::InfoType result;
    double distance;
    auto state = this->searcher_grid_binary->calculate(
        *(this->points), *(this->segments), this->dist_calculator, query,
        result, distance, 1.0);
    EXPECT_EQ(state, PrecisionState::Approximate);
}

// Location-specific test
TEST(NearestLocationGridFBSpecificTest, AlternativeNames_CorrectFirst) {
    auto dist_calculator = Distance::create("haversine");
    auto searcher = GenericNearestPoint<
        LocationInfo, PointsWrapperFB<LocationsData::Location>>::create("grid");

    auto item =
        LocationItem{LocationInfo(0, "Russia", "Moscow", -1, -1, {"Moskva"}),
                     Coordinates(55.75, 37.62)};
    auto locations = LocationTestTraitsFB::make_wrapper({item});

    SegmentsConverter seg_conv{};
    flatbuffers::FlatBufferBuilder seg_builder;
    SegmentInfo seg0(0, {55.0, 57.0}, {36.0, 42.0}, {}, {0});
    auto seg_offset0 = seg_conv.convert_to_fb(seg_builder, seg0);
    std::vector<flatbuffers::Offset<SegmentsData::SegmentInfo>> seg_vec = {
        seg_offset0};
    auto seg_root =
        seg_conv.create_root_fb(seg_builder, seg_builder.CreateVector(seg_vec));
    seg_builder.Finish(seg_root);

    const uint8_t* seg_buf = seg_builder.GetBufferPointer();
    size_t seg_size = seg_builder.GetSize();
    std::shared_ptr<char> seg_buffer(new char[seg_size],
                                     std::default_delete<char[]>());
    std::memcpy(seg_buffer.get(), seg_buf, seg_size);
    const auto* segs_fb = SegmentsData::GetSegments(seg_buffer.get());
    SegmentsWrapperFB segments(seg_buffer, segs_fb->data());

    Coordinates query(55.80, 37.70);
    LocationInfo result;
    double distance;
    searcher->calculate(*locations, segments, dist_calculator, query, result,
                        distance, 0.0);
    ASSERT_GE(result.alt_names.size(), 1u);
    EXPECT_EQ(result.alt_names[0], "Moskva");
}
