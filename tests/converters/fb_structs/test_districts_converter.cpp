// Copyright 2025 itlab-vision
#include <gtest/gtest.h>
#include <string>
#include <vector>
#include <utility>

#include "flatbuffers/flatbuffers.h"  // NOLINT
#include "districts_converter.hpp"    // NOLINT
extern "C" {
#include "tg.h"  // NOLINT
}

class DistrictsConverterTest : public ::testing::Test {
 protected:
    DistrictItem restored_item;
    DistrictsConverter converter{};

    void SetUp() override {
        DistrictItem original_item;
        original_item.first.id = 99;
        original_item.first.name_en = "Central";
        original_item.first.name = "Центральный";

        TgGeomPtr geom(tg_parse_wkt("POLYGON((0 0, 1 0, 1 1, 0 1, 0 0))"));
        original_item.second = TgGeomSharedPtr(std::move(geom));

        flatbuffers::FlatBufferBuilder builder;
        auto offset = converter.convert_to_fb(builder, original_item);
        std::vector<flatbuffers::Offset<DistrictsData::DistrictEntry>> vec = {
            offset};
        auto vec_offset = builder.CreateVector(vec);
        auto root = converter.create_root_fb(builder, vec_offset);
        builder.Finish(root);

        const auto* root_fb =
            DistrictsData::GetDistricts(builder.GetBufferPointer());
        const auto* item_fb = root_fb->data()->Get(0);
        restored_item = converter.convert_from_fb(item_fb);
    }
};

TEST_F(DistrictsConverterTest, IdCorrect) {
    EXPECT_EQ(restored_item.first.id, 99u);
}
TEST_F(DistrictsConverterTest, NameEnCorrect) {
    EXPECT_EQ(restored_item.first.name_en, "Central");
}
TEST_F(DistrictsConverterTest, NameCorrect) {
    EXPECT_EQ(restored_item.first.name, "Центральный");
}
TEST_F(DistrictsConverterTest, GeomNotNull) {
    EXPECT_NE(restored_item.second, nullptr);
}
TEST_F(DistrictsConverterTest, GeomIsPolygon) {
    EXPECT_EQ(tg_geom_typeof(restored_item.second.get()), TG_POLYGON);
}
