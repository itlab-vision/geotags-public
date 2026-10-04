// Copyright 2025 itlab-vision
#include <gtest/gtest.h>
#include <string>
#include <vector>

#include "flatbuffers/flatbuffers.h"  // NOLINT
#include "locations_converter.hpp"    // NOLINT

class LocationsConverterTest : public ::testing::Test {
 protected:
    LocationItem restored_item;
    LocationsConverter converter{};

    void SetUp() override {
        LocationItem original_item;
        original_item.first.location_id = 42;
        original_item.first.country = "Russia";
        original_item.first.city = "Kazan";
        original_item.first.region_id = 16;
        original_item.first.district_id = 5;
        original_item.first.alt_names = {"KZN", "Kazan-City"};
        original_item.first.attraction_db_path = "db/kzn.db";
        original_item.second.latitude = 55.79;
        original_item.second.longitude = 49.10;

        flatbuffers::FlatBufferBuilder builder;
        auto offset = converter.convert_to_fb(builder, original_item);
        std::vector<flatbuffers::Offset<LocationsData::Location>> vec = {
            offset};
        auto vec_offset = builder.CreateVector(vec);
        auto root = converter.create_root_fb(builder, vec_offset);
        builder.Finish(root);

        const auto* locs =
            LocationsData::GetLocations(builder.GetBufferPointer());
        const auto* loc = locs->data()->Get(0);
        restored_item = converter.convert_from_fb(loc);
    }
};

TEST_F(LocationsConverterTest, IdCorrect) {
    EXPECT_EQ(restored_item.first.location_id, 42u);
}
TEST_F(LocationsConverterTest, CountryCorrect) {
    EXPECT_EQ(restored_item.first.country, "Russia");
}
TEST_F(LocationsConverterTest, CityCorrect) {
    EXPECT_EQ(restored_item.first.city, "Kazan");
}
TEST_F(LocationsConverterTest, RegionIdCorrect) {
    EXPECT_EQ(restored_item.first.region_id, 16);
}
TEST_F(LocationsConverterTest, DistrictIdCorrect) {
    EXPECT_EQ(restored_item.first.district_id, 5);
}
TEST_F(LocationsConverterTest, AltNamesSizeCorrect) {
    EXPECT_EQ(restored_item.first.alt_names.size(), 2u);
}
TEST_F(LocationsConverterTest, AltNamesFirstCorrect) {
    EXPECT_EQ(restored_item.first.alt_names[0], "KZN");
}
TEST_F(LocationsConverterTest, AltNamesSecondCorrect) {
    EXPECT_EQ(restored_item.first.alt_names[1], "Kazan-City");
}
TEST_F(LocationsConverterTest, AttractionDbPathCorrect) {
    EXPECT_EQ(restored_item.first.attraction_db_path, "db/kzn.db");
}
TEST_F(LocationsConverterTest, LatitudeCorrect) {
    EXPECT_DOUBLE_EQ(restored_item.second.latitude, 55.79);
}
TEST_F(LocationsConverterTest, LongitudeCorrect) {
    EXPECT_DOUBLE_EQ(restored_item.second.longitude, 49.10);
}
