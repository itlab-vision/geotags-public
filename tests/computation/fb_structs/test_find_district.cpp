// Copyright 2025 itlab-vision
#include <gtest/gtest.h>
#include <cstring>
#include <memory>
#include <vector>
#include <string>
#include <stdexcept>
#include <utility>
#include <optional>

#include "district_finder.hpp"                // NOLINT
#include "coordinates.hpp"                    // NOLINT
#include "location_info.hpp"                  // NOLINT
#include "district_info.hpp"                  // NOLINT
#include "flatbuffers/flatbuffers.h"          // NOLINT
#include "districts_wrapper_fb.hpp"           // NOLINT
#include "district_neighbors_wrapper_fb.hpp"  // NOLINT
#include "districts_converter.hpp"            // NOLINT
#include "district_neighbors_converter.hpp"   // NOLINT
#include "auxiliary.hpp"                      // NOLINT

extern "C" {
#include "tg.h"  // NOLINT
}

class FindDistrictFBTest : public ::testing::Test {
 protected:
    void SetUp() override {
        // 1. Create main districts
        DistrictsConverter dist_conv{};
        flatbuffers::FlatBufferBuilder dist_builder;

        DistrictItem item1;
        item1.first = DistrictInfo(1, "Moscow Oblast", "Московская область");
        item1.second = TgGeomSharedPtr(
            TgGeomPtr(tg_parse_wkt("POLYGON((35.0 55.0, 37.0 55.0, 37.0 57.0, "
                                   "35.0 57.0, 35.0 55.0))")));

        DistrictItem item2;
        item2.first = DistrictInfo(2, "Tver Oblast", "Тверская область");
        item2.second = TgGeomSharedPtr(
            TgGeomPtr(tg_parse_wkt("POLYGON((40.0 50.0, 42.0 50.0, 42.0 52.0, "
                                   "40.0 52.0, 40.0 50.0))")));

        auto offset1 = dist_conv.convert_to_fb(dist_builder, item1);
        auto offset2 = dist_conv.convert_to_fb(dist_builder, item2);

        std::vector<flatbuffers::Offset<DistrictsData::DistrictEntry>>
            dist_vec = {offset1, offset2};
        auto root1 = dist_conv.create_root_fb(
            dist_builder, dist_builder.CreateVector(dist_vec));
        dist_builder.Finish(root1);

        const uint8_t* dist_buf = dist_builder.GetBufferPointer();
        size_t dist_size = dist_builder.GetSize();
        std::shared_ptr<char> dist_buffer(new char[dist_size],
                                          std::default_delete<char[]>());
        std::memcpy(dist_buffer.get(), dist_buf, dist_size);
        const auto* dists_fb = DistrictsData::GetDistricts(dist_buffer.get());
        districts =
            std::make_unique<DistrictsWrapperFB>(dist_buffer, dists_fb->data());

        // 2. Create neighbor districts
        flatbuffers::FlatBufferBuilder nd_builder;

        DistrictItem nd1;
        nd1.first = DistrictInfo(0, "D1", "D1");
        nd1.second = TgGeomSharedPtr(
            TgGeomPtr(tg_parse_wkt("POLYGON((0 0, 1 0, 1 1, 0 1, 0 0))")));

        DistrictItem nd2;
        nd2.first = DistrictInfo(1, "D2", "D2");
        nd2.second = TgGeomSharedPtr(
            TgGeomPtr(tg_parse_wkt("POLYGON((1 0, 2 0, 2 1, 1 1, 1 0))")));

        auto nd_offset1 = dist_conv.convert_to_fb(nd_builder, nd1);
        auto nd_offset2 = dist_conv.convert_to_fb(nd_builder, nd2);

        std::vector<flatbuffers::Offset<DistrictsData::DistrictEntry>> nd_vec =
            {nd_offset1, nd_offset2};
        auto root2 = dist_conv.create_root_fb(nd_builder,
                                              nd_builder.CreateVector(nd_vec));
        nd_builder.Finish(root2);

        const uint8_t* nd_buf = nd_builder.GetBufferPointer();
        size_t nd_size = nd_builder.GetSize();
        std::shared_ptr<char> nd_buffer(new char[nd_size],
                                        std::default_delete<char[]>());
        std::memcpy(nd_buffer.get(), nd_buf, nd_size);
        const auto* nd_fb = DistrictsData::GetDistricts(nd_buffer.get());
        neighbor_districts =
            std::make_unique<DistrictsWrapperFB>(nd_buffer, nd_fb->data());

        // 3. Create neighbors
        DistrictNeighborsConverter nbr_conv{};
        flatbuffers::FlatBufferBuilder nbr_builder;

        DistrictNeighborInfo nbr1{0, {1}};
        DistrictNeighborInfo nbr2{1, {0}};

        auto nbr_offset1 = nbr_conv.convert_to_fb(nbr_builder, nbr1);
        auto nbr_offset2 = nbr_conv.convert_to_fb(nbr_builder, nbr2);

        std::vector<
            flatbuffers::Offset<DistrictNeighborsData::DistrictNeighborInfo>>
            nbr_vec = {nbr_offset1, nbr_offset2};
        auto root3 = nbr_conv.create_root_fb(nbr_builder,
                                             nbr_builder.CreateVector(nbr_vec));
        nbr_builder.Finish(root3);

        const uint8_t* nbr_buf = nbr_builder.GetBufferPointer();
        size_t nbr_size = nbr_builder.GetSize();
        std::shared_ptr<char> nbr_buffer(new char[nbr_size],
                                         std::default_delete<char[]>());
        std::memcpy(nbr_buffer.get(), nbr_buf, nbr_size);
        const auto* nbr_fb =
            DistrictNeighborsData::GetDistrictNeighbors(nbr_buffer.get());
        neighbors = std::make_unique<DistrictNeighborsWrapperFB>(
            nbr_buffer, nbr_fb->data());

        finder_linear = DistrictFinder::create("linear");
        finder_neighbor = DistrictFinder::create("neighbors");
    }

    std::unique_ptr<DistrictsWrapperFB> districts;
    std::unique_ptr<DistrictsWrapperFB> neighbor_districts;
    std::unique_ptr<DistrictNeighborsWrapperFB> neighbors;
    Ptr<DistrictFinder> finder_linear = nullptr;
    Ptr<DistrictFinder> finder_neighbor = nullptr;
};

// --- Linear Finder Tests ---

TEST_F(FindDistrictFBTest, Linear_InsideD1_ReturnsExact) {
    Coordinates location(56.0, 36.0);
    DistrictInfo result;
    LocationInfo dummy_location{};
    auto state = finder_linear->find_district(*districts, std::nullopt,
                                              location, dummy_location, result);
    EXPECT_EQ(state, PrecisionState::Exact);
}

TEST_F(FindDistrictFBTest, Linear_InsideD1_CorrectId) {
    Coordinates location(56.0, 36.0);
    DistrictInfo result;
    LocationInfo dummy_location{};
    finder_linear->find_district(*districts, std::nullopt, location,
                                 dummy_location, result);
    EXPECT_EQ(result.id, 1u);
}

TEST_F(FindDistrictFBTest, Linear_InsideD1_CorrectNameEn) {
    Coordinates location(56.0, 36.0);
    DistrictInfo result;
    LocationInfo dummy_location{};
    finder_linear->find_district(*districts, std::nullopt, location,
                                 dummy_location, result);
    EXPECT_EQ(result.name_en, "Moscow Oblast");
}

TEST_F(FindDistrictFBTest, Linear_InsideD2_ReturnsExact) {
    Coordinates location(51.0, 41.0);
    DistrictInfo result;
    LocationInfo dummy_location{};
    auto state = finder_linear->find_district(*districts, std::nullopt,
                                              location, dummy_location, result);
    EXPECT_EQ(state, PrecisionState::Exact);
}

TEST_F(FindDistrictFBTest, Linear_InsideD2_CorrectId) {
    Coordinates location(51.0, 41.0);
    DistrictInfo result;
    LocationInfo dummy_location{};
    finder_linear->find_district(*districts, std::nullopt, location,
                                 dummy_location, result);
    EXPECT_EQ(result.id, 2u);
}

TEST_F(FindDistrictFBTest, Linear_Outside_ReturnsApproximate) {
    Coordinates location(39.0, 45.0);
    DistrictInfo result;
    LocationInfo dummy_location{};
    auto state = finder_linear->find_district(*districts, std::nullopt,
                                              location, dummy_location, result);
    EXPECT_EQ(state, PrecisionState::Approximate);
}

TEST_F(FindDistrictFBTest, Linear_EmptyList_ThrowsException) {
    std::shared_ptr<char> empty_buffer;
    // We pass empty data pointer.
    DistrictsWrapperFB empty_districts(empty_buffer, nullptr);
    Coordinates location(56.0, 36.0);
    DistrictInfo result;
    LocationInfo dummy_location{};
    EXPECT_THROW(finder_linear->find_district(empty_districts, std::nullopt,
                                              location, dummy_location, result),
                 std::runtime_error);
}

// --- Neighbor Finder Tests ---

TEST_F(FindDistrictFBTest, Neighbor_InsideBase_ReturnsExact) {
    Coordinates location(0.5, 0.5);  // Inside D1 (index 0)
    DistrictInfo result;
    LocationInfo nearest_location{};
    nearest_location.region_id = 0;  // Starts at D1
    auto state = finder_neighbor->find_district(
        *neighbor_districts, *neighbors, location, nearest_location, result);
    EXPECT_EQ(state, PrecisionState::Exact);
}

TEST_F(FindDistrictFBTest, Neighbor_InsideBase_CorrectId) {
    Coordinates location(0.5, 0.5);  // Inside D1 (index 0)
    DistrictInfo result;
    LocationInfo nearest_location{};
    nearest_location.region_id = 0;  // Starts at D1
    finder_neighbor->find_district(*neighbor_districts, *neighbors, location,
                                   nearest_location, result);
    EXPECT_EQ(result.id, 0u);
}

TEST_F(FindDistrictFBTest, Neighbor_InsideNeighbor_ReturnsExact) {
    Coordinates location(0.5, 1.5);  // Inside D2 (index 1)
    DistrictInfo result;
    LocationInfo nearest_location{};
    nearest_location.region_id =
        0;  // Starts at D1, should look at D2 which is neighbor
    auto state = finder_neighbor->find_district(
        *neighbor_districts, *neighbors, location, nearest_location, result);
    EXPECT_EQ(state, PrecisionState::Exact);
}

TEST_F(FindDistrictFBTest, Neighbor_InsideNeighbor_CorrectId) {
    Coordinates location(0.5, 1.5);  // Inside D2 (index 1)
    DistrictInfo result;
    LocationInfo nearest_location{};
    nearest_location.region_id =
        0;  // Starts at D1, should look at D2 which is neighbor
    finder_neighbor->find_district(*neighbor_districts, *neighbors, location,
                                   nearest_location, result);
    EXPECT_EQ(result.id, 1u);
}

TEST_F(FindDistrictFBTest, Neighbor_OutsideBoth_ReturnsApproximate) {
    Coordinates location(10.0, 10.0);
    DistrictInfo result;
    LocationInfo nearest_location{};
    nearest_location.region_id = 0;
    auto state = finder_neighbor->find_district(
        *neighbor_districts, *neighbors, location, nearest_location, result);
    EXPECT_EQ(state, PrecisionState::Approximate);
}

TEST_F(FindDistrictFBTest, Neighbor_EmptyDistricts_ThrowsException) {
    std::shared_ptr<char> empty_buffer;
    DistrictsWrapperFB empty_districts(empty_buffer, nullptr);
    Coordinates location(0.5, 0.5);
    DistrictInfo result;
    LocationInfo nearest_location{};
    nearest_location.region_id = 0;
    EXPECT_THROW(
        finder_neighbor->find_district(empty_districts, *neighbors, location,
                                       nearest_location, result),
        std::runtime_error);
}

TEST_F(FindDistrictFBTest, Neighbor_EmptyNeighbors_ThrowsException) {
    std::shared_ptr<char> empty_buffer;
    DistrictNeighborsWrapperFB empty_nbrs(empty_buffer, nullptr);
    Coordinates location(0.5, 0.5);
    DistrictInfo result;
    LocationInfo nearest_location{};
    nearest_location.region_id = 0;
    EXPECT_THROW(
        finder_neighbor->find_district(*neighbor_districts, empty_nbrs,
                                       location, nearest_location, result),
        std::runtime_error);
}
