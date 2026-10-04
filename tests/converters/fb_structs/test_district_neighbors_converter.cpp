// Copyright 2025 itlab-vision
#include <gtest/gtest.h>
#include <string>
#include <vector>

#include "flatbuffers/flatbuffers.h"         // NOLINT
#include "district_neighbors_converter.hpp"  // NOLINT

class DistrictNeighborsConverterTest : public ::testing::Test {
 protected:
    DistrictNeighborInfo restored_item;
    DistrictNeighborsConverter converter{};

    void SetUp() override {
        DistrictNeighborInfo original_item;
        original_item.district_id = 7;
        original_item.neighbors = {10, 11, 12};

        flatbuffers::FlatBufferBuilder builder;
        auto offset = converter.convert_to_fb(builder, original_item);
        std::vector<
            flatbuffers::Offset<DistrictNeighborsData::DistrictNeighborInfo>>
            vec = {offset};
        auto vec_offset = builder.CreateVector(vec);
        auto root = converter.create_root_fb(builder, vec_offset);
        builder.Finish(root);

        const auto* root_fb = DistrictNeighborsData::GetDistrictNeighbors(
            builder.GetBufferPointer());
        const auto* item_fb = root_fb->data()->Get(0);
        restored_item = converter.convert_from_fb(item_fb);
    }
};

TEST_F(DistrictNeighborsConverterTest, DistrictIdCorrect) {
    EXPECT_EQ(restored_item.district_id, 7u);
}
TEST_F(DistrictNeighborsConverterTest, NeighborsSizeCorrect) {
    EXPECT_EQ(restored_item.neighbors.size(), 3u);
}
TEST_F(DistrictNeighborsConverterTest, NeighborsFirstElementCorrect) {
    EXPECT_EQ(restored_item.neighbors[0], 10u);
}
TEST_F(DistrictNeighborsConverterTest, NeighborsSecondElementCorrect) {
    EXPECT_EQ(restored_item.neighbors[1], 11u);
}
TEST_F(DistrictNeighborsConverterTest, NeighborsThirdElementCorrect) {
    EXPECT_EQ(restored_item.neighbors[2], 12u);
}
