// Copyright 2025 itlab-vision
#pragma once

#include <utility>
#include <vector>
#include <string>

#include "district_neighbors_generated.hpp"  // NOLINT
#include "district_neighbor_info.hpp"        // NOLINT
#include "converter_interface.hpp"           // NOLINT

class DistrictNeighborsConverter
    : public ConverterInterface<DistrictNeighborInfo,
                                DistrictNeighborsData::DistrictNeighborInfo,
                                DistrictNeighborsData::DistrictNeighbors> {
 public:
    DistrictNeighborsConverter() = default;

    flatbuffers::Offset<DistrictNeighborsData::DistrictNeighborInfo>
    convert_to_fb(flatbuffers::FlatBufferBuilder& builder,
                  const DistrictNeighborInfo& neighbor) override;

    DistrictNeighborInfo convert_from_fb(
        const DistrictNeighborsData::DistrictNeighborInfo* neighbor) override;

    flatbuffers::Offset<DistrictNeighborsData::DistrictNeighbors>
    create_root_fb(
        flatbuffers::FlatBufferBuilder& builder,
        flatbuffers::Offset<flatbuffers::Vector<
            flatbuffers::Offset<DistrictNeighborsData::DistrictNeighborInfo>>>
            data_vec) override {
        return DistrictNeighborsData::CreateDistrictNeighbors(builder,
                                                              data_vec);
    }
};
