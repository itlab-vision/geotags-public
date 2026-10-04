// Copyright 2025 itlab-vision

#include <vector>
#include <string>
#include <utility>

#include "locations_converter.hpp"  // NOLINT

flatbuffers::Offset<LocationsData::Location> LocationsConverter::convert_to_fb(
    flatbuffers::FlatBufferBuilder& builder, const LocationItem& location) {
    auto& info = location.first;
    auto& coords = location.second;

    auto location_id = info.location_id;
    auto country = builder.CreateString(info.country);
    auto city = builder.CreateString(info.city);
    auto region_id = info.region_id;
    auto district_id = info.district_id;
    std::vector<flatbuffers::Offset<flatbuffers::String>> alt_names_vec;
    for (const auto& name : info.alt_names)
        alt_names_vec.push_back(builder.CreateString(name));
    auto alt_names = builder.CreateVector(alt_names_vec);
    auto attraction_db_path = builder.CreateString(info.attraction_db_path);

    auto info_fb = LocationsData::CreateLocationInfo(
        builder, location_id, country, city, region_id, district_id, alt_names,
        attraction_db_path);
    auto coords_fb =
        LocationsData::Coordinates(coords.latitude, coords.longitude);
    auto loc_fb = LocationsData::CreateLocation(builder, info_fb, &coords_fb);

    return loc_fb;
}

LocationItem LocationsConverter::convert_from_fb(
    const LocationsData::Location* location) {
    return LocationItem{LocationInfo(location->info()),
                        Coordinates(location->coords()->latitude(),
                                    location->coords()->longitude())};
}
