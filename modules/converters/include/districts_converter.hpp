// Copyright 2025 itlab-vision
#pragma once

#include <utility>
#include <vector>
#include <string>

#include "auxiliary.hpp"            // NOLINT
#include "districts_generated.hpp"  // NOLINT
#include "district_info.hpp"        // NOLINT
#include "converter_interface.hpp"  // NOLINT
#include "districts_utils.hpp"      // NOLINT

extern "C" {
#include "tg.h"  // NOLINT
}

using DistrictItem = std::pair<DistrictInfo, TgGeomSharedPtr>;

class DistrictsConverter
    : public ConverterInterface<DistrictItem, DistrictsData::DistrictEntry,
                                DistrictsData::Districts> {
 private:
    static flatbuffers::Offset<DistrictsData::DistrictEntry> serialize_district(
        flatbuffers::FlatBufferBuilder& builder, const DistrictInfo& info,
        const tg_geom* geom);

    static flatbuffers::Offset<DistrictsData::DistrictInfo> serialize_info(
        flatbuffers::FlatBufferBuilder& builder, const DistrictInfo& info);

    static flatbuffers::Offset<void> serialize_geom(
        flatbuffers::FlatBufferBuilder& b, const tg_geom* geom);

    static flatbuffers::Offset<DistrictsData::Ring> serialize_ring(
        flatbuffers::FlatBufferBuilder& builder, const tg_ring* ring);

    static flatbuffers::Offset<DistrictsData::Polygon> serialize_polygon(
        flatbuffers::FlatBufferBuilder& builder, const tg_poly* poly);

    static flatbuffers::Offset<DistrictsData::MultiPolygon>
    serialize_multipolygon(flatbuffers::FlatBufferBuilder& builder,
                           const tg_geom* geom);

 public:
    DistrictsConverter() = default;

    flatbuffers::Offset<DistrictsData::DistrictEntry> convert_to_fb(
        flatbuffers::FlatBufferBuilder& builder,
        const DistrictItem& district) override;

    DistrictItem convert_from_fb(
        const DistrictsData::DistrictEntry* district_entry) override;

    flatbuffers::Offset<DistrictsData::Districts> create_root_fb(
        flatbuffers::FlatBufferBuilder& builder,
        flatbuffers::Offset<flatbuffers::Vector<
            flatbuffers::Offset<DistrictsData::DistrictEntry>>>
            data_vec) override {
        return DistrictsData::CreateDistricts(builder, data_vec);
    }
};
