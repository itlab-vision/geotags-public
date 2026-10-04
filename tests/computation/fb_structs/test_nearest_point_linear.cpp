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
#include "locations_converter.hpp"    // NOLINT
#include "attractions_converter.hpp"  // NOLINT

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
class NearestPointLinearFBTest : public ::testing::Test {
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
        points = T::make_wrapper(data);
    }

    Ptr<Distance> dist_calculator;
    Ptr<GenericNearestPoint<typename T::InfoType, typename T::WrapperType>>
        searcher;
    std::unique_ptr<typename T::WrapperType> points;
};

using TestTypes =
    ::testing::Types<LocationTestTraitsFB, AttractionTestTraitsFB>;
TYPED_TEST_SUITE(NearestPointLinearFBTest, TestTypes);

TYPED_TEST(NearestPointLinearFBTest, Basic_ReturnsExact) {
    Coordinates query(55.80, 37.70);
    typename TypeParam::InfoType result;
    double distance;
    auto state = this->searcher->calculate(*(this->points), std::nullopt,
                                           this->dist_calculator, query, result,
                                           distance, 0.0);
    EXPECT_EQ(state, PrecisionState::Exact);
}

TYPED_TEST(NearestPointLinearFBTest, Basic_CorrectName) {
    Coordinates query(55.80, 37.70);
    typename TypeParam::InfoType result;
    double distance;
    this->searcher->calculate(*(this->points), std::nullopt,
                              this->dist_calculator, query, result, distance,
                              0.0);
    EXPECT_EQ(TypeParam::get_name(result), "Moscow");
}

TYPED_TEST(NearestPointLinearFBTest, Basic_DistanceIsPositive) {
    Coordinates query(55.80, 37.70);
    typename TypeParam::InfoType result;
    double distance;
    this->searcher->calculate(*(this->points), std::nullopt,
                              this->dist_calculator, query, result, distance,
                              0.0);
    EXPECT_GT(distance, 0.0);
}

TYPED_TEST(NearestPointLinearFBTest, ExactMatch_DistanceIsZero) {
    Coordinates query(55.75, 37.62);
    typename TypeParam::InfoType result;
    double distance;
    this->searcher->calculate(*(this->points), std::nullopt,
                              this->dist_calculator, query, result, distance,
                              0.0);
    EXPECT_DOUBLE_EQ(distance, 0.0);
}

TYPED_TEST(NearestPointLinearFBTest, ExactMatch_CorrectName) {
    Coordinates query(55.75, 37.62);
    typename TypeParam::InfoType result;
    double distance;
    this->searcher->calculate(*(this->points), std::nullopt,
                              this->dist_calculator, query, result, distance,
                              0.0);
    EXPECT_EQ(TypeParam::get_name(result), "Moscow");
}

TYPED_TEST(NearestPointLinearFBTest, WithThreshold_ReturnsApproximate) {
    Coordinates query(60.0, 40.0);
    typename TypeParam::InfoType result;
    double distance;
    auto state = this->searcher->calculate(*(this->points), std::nullopt,
                                           this->dist_calculator, query, result,
                                           distance, 1.0);
    EXPECT_EQ(state, PrecisionState::Approximate);
}

// Location-specific test
TEST(NearestLocationLinearFBSpecificTest, AlternativeNames_CorrectFirst) {
    auto dist_calculator = Distance::create("haversine");
    auto searcher = GenericNearestPoint<
        LocationInfo,
        PointsWrapperFB<LocationsData::Location>>::create("linear");

    auto item =
        LocationItem{LocationInfo(0, "Russia", "Moscow", -1, -1, {"Moskva"}),
                     Coordinates(55.75, 37.62)};
    auto locations = LocationTestTraitsFB::make_wrapper({item});

    Coordinates query(55.80, 37.70);
    LocationInfo result;
    double distance;
    searcher->calculate(*locations, std::nullopt, dist_calculator, query,
                        result, distance, 0.0);
    ASSERT_GE(result.alt_names.size(), 1u);
    EXPECT_EQ(result.alt_names[0], "Moskva");
}
