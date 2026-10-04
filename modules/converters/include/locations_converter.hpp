// Copyright 2025 itlab-vision
#pragma once

#include <utility>
#include <vector>
#include <string>

#include "locations_generated.hpp"  // NOLINT
#include "location_info.hpp"        // NOLINT
#include "coordinates.hpp"          // NOLINT
#include "converter_interface.hpp"  // NOLINT

using LocationItem = std::pair<LocationInfo, Coordinates>;

class LocationsConverter
    : public ConverterInterface<LocationItem, LocationsData::Location,
                                LocationsData::Locations> {
 public:
    LocationsConverter() = default;

    flatbuffers::Offset<LocationsData::Location> convert_to_fb(
        flatbuffers::FlatBufferBuilder& builder,
        const LocationItem& location) override;

    LocationItem convert_from_fb(
        const LocationsData::Location* location) override;

    flatbuffers::Offset<LocationsData::Locations> create_root_fb(
        flatbuffers::FlatBufferBuilder& builder,
        flatbuffers::Offset<
            flatbuffers::Vector<flatbuffers::Offset<LocationsData::Location>>>
            data_vec) override {
        return LocationsData::CreateLocations(builder, data_vec);
    }
};
