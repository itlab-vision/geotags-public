// Copyright 2025 itlab-vision
#include <gtest/gtest.h>
#include <cstring>
#include <memory>
#include <vector>
#include <utility>

#include "flatbuffers/flatbuffers.h"          // NOLINT
#include "district_neighbors_wrapper_fb.hpp"  // NOLINT
#include "district_neighbors_converter.hpp"   // NOLINT

class DistrictNeighborsWrapperFBTest : public ::testing::Test {
 protected:
    DistrictNeighborsWrapperFB* wrapper;

    void SetUp() override {
        DistrictNeighborsConverter converter{};

        DistrictNeighborInfo item1{5, {6, 7}};
        DistrictNeighborInfo item2{8, {9}};

        flatbuffers::FlatBufferBuilder builder;
        auto offset1 = converter.convert_to_fb(builder, item1);
        auto offset2 = converter.convert_to_fb(builder, item2);

        std::vector<
            flatbuffers::Offset<DistrictNeighborsData::DistrictNeighborInfo>>
            vec = {offset1, offset2};
        auto vec_offset = builder.CreateVector(vec);
        auto root = converter.create_root_fb(builder, vec_offset);
        builder.Finish(root);

        const uint8_t* buf = builder.GetBufferPointer();
        size_t size = builder.GetSize();
        std::shared_ptr<char> buffer(new char[size],
                                     std::default_delete<char[]>());
        std::memcpy(buffer.get(), buf, size);

        const auto* root_fb =
            DistrictNeighborsData::GetDistrictNeighbors(buffer.get());
        wrapper = new DistrictNeighborsWrapperFB(buffer, root_fb->data());
    }

    void TearDown() override { delete wrapper; }
};

TEST_F(DistrictNeighborsWrapperFBTest, SizeCorrect) {
    EXPECT_EQ(wrapper->size(), 2u);
}
TEST_F(DistrictNeighborsWrapperFBTest, NeighborsSizeCorrect) {
    std::vector<unsigned int> nbrs;
    wrapper->get_nbrs(0, nbrs);
    EXPECT_EQ(nbrs.size(), 2u);
}
TEST_F(DistrictNeighborsWrapperFBTest, NeighborsFirstElementCorrect) {
    std::vector<unsigned int> nbrs;
    wrapper->get_nbrs(0, nbrs);
    EXPECT_EQ(nbrs[0], 6u);
}
