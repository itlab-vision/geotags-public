// Copyright 2025 itlab-vision

#include <vector>
#include <utility>

#include "district_neighbors_converter.hpp"  // NOLINT

flatbuffers::Offset<DistrictNeighborsData::DistrictNeighborInfo>
DistrictNeighborsConverter::convert_to_fb(
    flatbuffers::FlatBufferBuilder& builder,
    const DistrictNeighborInfo& neighbor) {
    auto district_id = neighbor.district_id;
    auto neighbors_vector = builder.CreateVector(neighbor.neighbors);

    auto neighbor_info_fb = DistrictNeighborsData::CreateDistrictNeighborInfo(
        builder, district_id, neighbors_vector);

    return neighbor_info_fb;
}

DistrictNeighborInfo DistrictNeighborsConverter::convert_from_fb(
    const DistrictNeighborsData::DistrictNeighborInfo* neighbor) {
    std::vector<unsigned int> neighbors;
    if (auto fb_nbrs = neighbor->neighbors()) {
        neighbors.assign(fb_nbrs->begin(), fb_nbrs->end());
    }

    return DistrictNeighborInfo(neighbor->district_id(), std::move(neighbors));
}
