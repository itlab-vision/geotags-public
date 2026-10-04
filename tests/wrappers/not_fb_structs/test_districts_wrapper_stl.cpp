// Copyright 2025 itlab-vision
#include <gtest/gtest.h>
#include <vector>
#include <utility>

#include "districts_wrapper_stl.hpp"  // NOLINT
extern "C" {
#include "tg.h"  // NOLINT
}

class DistrictsWrapperSTLTest : public ::testing::Test {
 protected:
    std::vector<DistrictItem> data;
    DistrictsWrapperSTL* wrapper;

    void SetUp() override {
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

        data.push_back(std::move(item1));
        data.push_back(std::move(item2));

        wrapper = new DistrictsWrapperSTL(data);
    }

    void TearDown() override { delete wrapper; }
};

TEST_F(DistrictsWrapperSTLTest, SizeCorrect) { EXPECT_EQ(wrapper->size(), 2u); }
TEST_F(DistrictsWrapperSTLTest, GetInfoCorrect) {
    EXPECT_EQ(wrapper->get_info(0).id, 1u);
}
TEST_F(DistrictsWrapperSTLTest, GetInfoIdCorrect) {
    EXPECT_EQ(wrapper->get_info_id(1), 2u);
}
TEST_F(DistrictsWrapperSTLTest, GetGeomNotNull) {
    EXPECT_NE(wrapper->get_geom(0), nullptr);
}
