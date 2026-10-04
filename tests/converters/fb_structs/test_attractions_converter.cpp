// Copyright 2025 itlab-vision
#include <gtest/gtest.h>
#include <string>
#include <vector>
#include <unordered_map>

#include "flatbuffers/flatbuffers.h"  // NOLINT
#include "attractions_converter.hpp"  // NOLINT

class AttractionsConverterTest : public ::testing::Test {
 protected:
    AttractionItem restored_item;
    AttractionsConverter converter{};

    void SetUp() override {
        AttractionItem original_item;
        original_item.first.type = "museum";
        original_item.first.name = "Hermitage";
        original_item.first.extra_fields = {{"entry", "free"},
                                            {"visited", "yes"}};
        original_item.second.latitude = 59.93;
        original_item.second.longitude = 30.31;

        flatbuffers::FlatBufferBuilder builder;
        auto offset = converter.convert_to_fb(builder, original_item);
        std::vector<flatbuffers::Offset<AttractionsData::Attraction>> vec = {
            offset};
        auto vec_offset = builder.CreateVector(vec);
        auto root = converter.create_root_fb(builder, vec_offset);
        builder.Finish(root);

        const auto* root_fb =
            AttractionsData::GetAttractions(builder.GetBufferPointer());
        const auto* item_fb = root_fb->data()->Get(0);
        restored_item = converter.convert_from_fb(item_fb);
    }
};

TEST_F(AttractionsConverterTest, TypeCorrect) {
    EXPECT_EQ(restored_item.first.type, "museum");
}
TEST_F(AttractionsConverterTest, NameCorrect) {
    EXPECT_EQ(restored_item.first.name, "Hermitage");
}
TEST_F(AttractionsConverterTest, LatitudeCorrect) {
    EXPECT_DOUBLE_EQ(restored_item.second.latitude, 59.93);
}
TEST_F(AttractionsConverterTest, LongitudeCorrect) {
    EXPECT_DOUBLE_EQ(restored_item.second.longitude, 30.31);
}
TEST_F(AttractionsConverterTest, ExtraFieldsSizeCorrect) {
    EXPECT_EQ(restored_item.first.extra_fields.size(), 2u);
}
TEST_F(AttractionsConverterTest, ExtraFieldsEntryCorrect) {
    EXPECT_EQ(restored_item.first.extra_fields.count("entry")
                  ? restored_item.first.extra_fields.at("entry")
                  : "",
              "free");
}
TEST_F(AttractionsConverterTest, ExtraFieldsVisitedCorrect) {
    EXPECT_EQ(restored_item.first.extra_fields.count("visited")
                  ? restored_item.first.extra_fields.at("visited")
                  : "",
              "yes");
}
