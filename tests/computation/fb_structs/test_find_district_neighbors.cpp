// Copyright 2025 itlab-vision
#include <gtest/gtest.h>
#include <vector>
#include <string>
#include <algorithm>
#include <cstring>
#include <map>
#include <memory>
#include <utility>

#include "district_neighbors_utils.hpp"  // NOLINT
#include "flatbuffers/flatbuffers.h"     // NOLINT
#include "districts_wrapper_fb.hpp"      // NOLINT
#include "districts_converter.hpp"       // NOLINT
#include "district_info.hpp"             // NOLINT
#include "auxiliary.hpp"                 // NOLINT

extern "C" {
#include "tg.h"  // NOLINT
}

class FindDistrictNeighborsFBTest : public ::testing::Test {
 protected:
    void SetUp() override {
        // District 1: (0,0)-(1,1)
        r1_geom = TgGeomSharedPtr(
            TgGeomPtr(tg_parse_wkt("POLYGON((0 0, 1 0, 1 1, 0 1, 0 0))")));
        // District 2: (1,0)-(2,1) (Touches D1 at x=1)
        r2_geom = TgGeomSharedPtr(
            TgGeomPtr(tg_parse_wkt("POLYGON((1 0, 2 0, 2 1, 1 1, 1 0))")));
        // District 3: (2,0)-(3,1) (Touches D2 at x=2, Disjoint from D1)
        r3_geom = TgGeomSharedPtr(
            TgGeomPtr(tg_parse_wkt("POLYGON((2 0, 3 0, 3 1, 2 1, 2 0))")));
        // District 4: (10,10)-(11,11) (Disjoint from all)
        r4_geom = TgGeomSharedPtr(TgGeomPtr(
            tg_parse_wkt("POLYGON((10 10, 11 10, 11 11, 10 11, 10 10))")));

        DistrictsConverter converter{};
        flatbuffers::FlatBufferBuilder builder;

        DistrictItem item1{DistrictInfo(1, "District 1", "Регион 1"), r1_geom};
        DistrictItem item2{DistrictInfo(2, "District 2", "Регион 2"), r2_geom};
        DistrictItem item3{DistrictInfo(3, "District 3", "Регион 3"), r3_geom};
        DistrictItem item4{DistrictInfo(4, "District 4", "Регион 4"), r4_geom};

        auto offset1 = converter.convert_to_fb(builder, item1);
        auto offset2 = converter.convert_to_fb(builder, item2);
        auto offset3 = converter.convert_to_fb(builder, item3);
        auto offset4 = converter.convert_to_fb(builder, item4);

        std::vector<flatbuffers::Offset<DistrictsData::DistrictEntry>> vec = {
            offset1, offset2, offset3, offset4};
        auto root =
            converter.create_root_fb(builder, builder.CreateVector(vec));
        builder.Finish(root);

        const uint8_t* buf = builder.GetBufferPointer();
        size_t size = builder.GetSize();
        std::shared_ptr<char> buffer(new char[size],
                                     std::default_delete<char[]>());
        std::memcpy(buffer.get(), buf, size);
        const auto* dists_fb = DistrictsData::GetDistricts(buffer.get());

        districts =
            std::make_unique<DistrictsWrapperFB>(buffer, dists_fb->data());
    }

    TgGeomSharedPtr r1_geom = nullptr;
    TgGeomSharedPtr r2_geom = nullptr;
    TgGeomSharedPtr r3_geom = nullptr;
    TgGeomSharedPtr r4_geom = nullptr;
    std::unique_ptr<DistrictsWrapperFB> districts;
};

TEST_F(FindDistrictNeighborsFBTest, EmptyInput_ReturnsEmptyMap) {
    std::shared_ptr<char> empty_buffer;
    DistrictsWrapperFB empty_districts(empty_buffer, nullptr);
    auto result = find_district_neighbors(empty_districts);
    EXPECT_TRUE(result.empty());
}

TEST_F(FindDistrictNeighborsFBTest, SingleDistrict_MapSize) {
    DistrictsConverter converter{};
    flatbuffers::FlatBufferBuilder builder;
    DistrictItem item1{DistrictInfo(1, "District 1", "Регион 1"), r1_geom};
    auto offset1 = converter.convert_to_fb(builder, item1);
    std::vector<flatbuffers::Offset<DistrictsData::DistrictEntry>> vec = {
        offset1};
    auto root = converter.create_root_fb(builder, builder.CreateVector(vec));
    builder.Finish(root);

    const uint8_t* buf = builder.GetBufferPointer();
    size_t size = builder.GetSize();
    std::shared_ptr<char> buffer(new char[size], std::default_delete<char[]>());
    std::memcpy(buffer.get(), buf, size);
    const auto* dists_fb = DistrictsData::GetDistricts(buffer.get());
    DistrictsWrapperFB single_district(buffer, dists_fb->data());

    auto result = find_district_neighbors(single_district);
    EXPECT_EQ(result.size(), 1u);
}

TEST_F(FindDistrictNeighborsFBTest, SingleDistrict_HasNoNeighbors) {
    DistrictsConverter converter{};
    flatbuffers::FlatBufferBuilder builder;
    DistrictItem item1{DistrictInfo(1, "District 1", "Регион 1"), r1_geom};
    auto offset1 = converter.convert_to_fb(builder, item1);
    std::vector<flatbuffers::Offset<DistrictsData::DistrictEntry>> vec = {
        offset1};
    auto root = converter.create_root_fb(builder, builder.CreateVector(vec));
    builder.Finish(root);

    const uint8_t* buf = builder.GetBufferPointer();
    size_t size = builder.GetSize();
    std::shared_ptr<char> buffer(new char[size], std::default_delete<char[]>());
    std::memcpy(buffer.get(), buf, size);
    const auto* dists_fb = DistrictsData::GetDistricts(buffer.get());
    DistrictsWrapperFB single_district(buffer, dists_fb->data());

    auto result = find_district_neighbors(single_district);
    EXPECT_TRUE(result[1].empty());
}

TEST_F(FindDistrictNeighborsFBTest, Complex_MapSize) {
    auto result = find_district_neighbors(*districts);
    EXPECT_EQ(result.size(), 4u);
}

TEST_F(FindDistrictNeighborsFBTest, D1_NeighborCount) {
    auto result = find_district_neighbors(*districts);
    EXPECT_EQ(result[1].size(), 1u);
}

TEST_F(FindDistrictNeighborsFBTest, D1_HasNeighbor_D2) {
    auto result = find_district_neighbors(*districts);
    const auto& neighbors = result[1];
    EXPECT_NE(std::find(neighbors.begin(), neighbors.end(), 2u),
              neighbors.end());
}

TEST_F(FindDistrictNeighborsFBTest, D2_NeighborCount) {
    auto result = find_district_neighbors(*districts);
    EXPECT_EQ(result[2].size(), 2u);
}

TEST_F(FindDistrictNeighborsFBTest, D2_HasNeighbor_D1) {
    auto result = find_district_neighbors(*districts);
    const auto& neighbors = result[2];
    EXPECT_NE(std::find(neighbors.begin(), neighbors.end(), 1u),
              neighbors.end());
}

TEST_F(FindDistrictNeighborsFBTest, D2_HasNeighbor_D3) {
    auto result = find_district_neighbors(*districts);
    const auto& neighbors = result[2];
    EXPECT_NE(std::find(neighbors.begin(), neighbors.end(), 3u),
              neighbors.end());
}

TEST_F(FindDistrictNeighborsFBTest, D4_NeighborCount) {
    auto result = find_district_neighbors(*districts);
    EXPECT_TRUE(result[4].empty());
}

TEST_F(FindDistrictNeighborsFBTest, PointTouching_AreNeighbors) {
    TgGeomSharedPtr s1(
        TgGeomPtr(tg_parse_wkt("POLYGON((0 0, 1 0, 1 1, 0 1, 0 0))")));
    TgGeomSharedPtr s2(
        TgGeomPtr(tg_parse_wkt("POLYGON((1 1, 2 1, 2 2, 1 2, 1 1))")));

    DistrictsConverter converter{};
    flatbuffers::FlatBufferBuilder builder;
    DistrictItem item1{DistrictInfo(10, "S1", "S1"), s1};
    DistrictItem item2{DistrictInfo(11, "S2", "S2"), s2};
    auto offset1 = converter.convert_to_fb(builder, item1);
    auto offset2 = converter.convert_to_fb(builder, item2);
    std::vector<flatbuffers::Offset<DistrictsData::DistrictEntry>> vec = {
        offset1, offset2};
    auto root = converter.create_root_fb(builder, builder.CreateVector(vec));
    builder.Finish(root);

    const uint8_t* buf = builder.GetBufferPointer();
    size_t size = builder.GetSize();
    std::shared_ptr<char> buffer(new char[size], std::default_delete<char[]>());
    std::memcpy(buffer.get(), buf, size);
    const auto* dists_fb = DistrictsData::GetDistricts(buffer.get());
    DistrictsWrapperFB point_districts(buffer, dists_fb->data());

    auto result = find_district_neighbors(point_districts);
    const auto& neighbors = result[10];
    EXPECT_NE(std::find(neighbors.begin(), neighbors.end(), 11u),
              neighbors.end());
}
