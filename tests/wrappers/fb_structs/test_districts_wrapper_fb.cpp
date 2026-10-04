// Copyright 2025 itlab-vision
#include <gtest/gtest.h>
#include <cstring>
#include <memory>
#include <vector>
#include <utility>

#include "flatbuffers/flatbuffers.h"  // NOLINT
#include "districts_wrapper_fb.hpp"   // NOLINT
#include "districts_converter.hpp"    // NOLINT
extern "C" {
#include "tg.h"  // NOLINT
}

class DistrictsWrapperFBTest : public ::testing::Test {
 protected:
    flatbuffers::FlatBufferBuilder builder;
    DistrictsWrapperFB* wrapper;

    void SetUp() override {
        DistrictsConverter converter{};

        DistrictItem item1;
        item1.first.id = 1;
        item1.first.name_en = "D1";
        TgGeomPtr geom1(tg_parse_wkt("POLYGON((0 0, 1 0, 1 1, 0 1, 0 0))"));
        item1.second = TgGeomSharedPtr(std::move(geom1));

        DistrictItem item2;
        item2.first.id = 2;
        item2.first.name_en = "D2";
        TgGeomPtr geom2(tg_parse_wkt("POLYGON((2 2, 3 2, 3 3, 2 3, 2 2))"));
        item2.second = TgGeomSharedPtr(std::move(geom2));

        auto offset1 = converter.convert_to_fb(builder, item1);
        auto offset2 = converter.convert_to_fb(builder, item2);

        std::vector<flatbuffers::Offset<DistrictsData::DistrictEntry>> vec = {
            offset1, offset2};
        auto vec_offset = builder.CreateVector(vec);
        auto root = converter.create_root_fb(builder, vec_offset);
        builder.Finish(root);

        const uint8_t* buf = builder.GetBufferPointer();
        size_t size = builder.GetSize();
        std::shared_ptr<char> buffer(new char[size],
                                     std::default_delete<char[]>());
        std::memcpy(buffer.get(), buf, size);

        const auto* dists = DistrictsData::GetDistricts(buffer.get());
        wrapper = new DistrictsWrapperFB(buffer, dists->data());
    }

    void TearDown() override { delete wrapper; }
};

TEST_F(DistrictsWrapperFBTest, SizeCorrect) { EXPECT_EQ(wrapper->size(), 2u); }
TEST_F(DistrictsWrapperFBTest, GetInfoCorrect) {
    EXPECT_EQ(wrapper->get_info(0).id, 1u);
}
TEST_F(DistrictsWrapperFBTest, GetInfoIdCorrect) {
    EXPECT_EQ(wrapper->get_info_id(1), 2u);
}
TEST_F(DistrictsWrapperFBTest, GetGeomNotNull) {
    EXPECT_NE(wrapper->get_geom(0), nullptr);
}
