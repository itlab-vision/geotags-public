// Copyright 2025 itlab-vision
#include <gtest/gtest.h>
#include <cstring>
#include <memory>
#include <vector>
#include <string>
#include <utility>

#include "flatbuffers/flatbuffers.h"  // NOLINT
#include "points_wrapper_fb.hpp"      // NOLINT
#include "locations_converter.hpp"    // NOLINT

class PointsWrapperFBTest : public ::testing::Test {
 protected:
    flatbuffers::FlatBufferBuilder builder;
    PointsWrapperFB<LocationsData::Location>* wrapper;

    void SetUp() override {
        LocationsConverter converter{};

        LocationItem item1;
        item1.first.location_id = 1;
        item1.first.city = "MOW";
        item1.second.latitude = 55.75;
        item1.second.longitude = 37.61;

        LocationItem item2;
        item2.first.location_id = 2;
        item2.first.city = "SPB";
        item2.second.latitude = 59.93;
        item2.second.longitude = 30.33;

        auto offset1 = converter.convert_to_fb(builder, item1);
        auto offset2 = converter.convert_to_fb(builder, item2);

        std::vector<flatbuffers::Offset<LocationsData::Location>> vec = {
            offset1, offset2};
        auto vec_offset = builder.CreateVector(vec);
        auto root = converter.create_root_fb(builder, vec_offset);
        builder.Finish(root);

        const uint8_t* buf = builder.GetBufferPointer();
        size_t size = builder.GetSize();
        std::shared_ptr<char> buffer(new char[size],
                                     std::default_delete<char[]>());
        std::memcpy(buffer.get(), buf, size);

        const auto* locs = LocationsData::GetLocations(buffer.get());
        wrapper =
            new PointsWrapperFB<LocationsData::Location>(buffer, locs->data());
    }

    void TearDown() override { delete wrapper; }
};

TEST_F(PointsWrapperFBTest, SizeCorrect) { EXPECT_EQ(wrapper->size(), 2u); }
TEST_F(PointsWrapperFBTest, GetInfoCorrect) {
    EXPECT_EQ(wrapper->get_info(0)->location_id(), 1u);
}
TEST_F(PointsWrapperFBTest, GetLatFirstCorrect) {
    EXPECT_DOUBLE_EQ(wrapper->get_lat(0), 55.75);
}
TEST_F(PointsWrapperFBTest, GetLonFirstCorrect) {
    EXPECT_DOUBLE_EQ(wrapper->get_lon(0), 37.61);
}
TEST_F(PointsWrapperFBTest, GetLatSecondCorrect) {
    EXPECT_DOUBLE_EQ(wrapper->get_lat(1), 59.93);
}
TEST_F(PointsWrapperFBTest, GetLonSecondCorrect) {
    EXPECT_DOUBLE_EQ(wrapper->get_lon(1), 30.33);
}
