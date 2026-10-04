// Copyright 2025 itlab-vision

#include <vector>
#include <string>
#include <utility>

#include "attractions_converter.hpp"  // NOLINT

flatbuffers::Offset<AttractionsData::Attraction>
AttractionsConverter::convert_to_fb(flatbuffers::FlatBufferBuilder& builder,
                                    const AttractionItem& attraction) {
    auto& info = attraction.first;
    auto& coords = attraction.second;

    auto type = builder.CreateString(info.type);
    auto name = builder.CreateString(info.name);

    std::vector<flatbuffers::Offset<AttractionsData::ExtraField>>
        extra_fields_vec;
    for (const auto& [key, value] : info.extra_fields) {
        auto key_fb = builder.CreateString(key);
        auto value_fb = builder.CreateString(value);
        auto field_fb =
            AttractionsData::CreateExtraField(builder, key_fb, value_fb);
        extra_fields_vec.push_back(field_fb);
    }
    auto extra_fields = builder.CreateVectorOfSortedTables(&extra_fields_vec);

    auto coords_fb =
        AttractionsData::Coordinates(coords.latitude, coords.longitude);
    auto attraction_info_fb = AttractionsData::CreateAttractionInfo(
        builder, type, name, extra_fields);
    auto attraction_fb = AttractionsData::CreateAttraction(
        builder, attraction_info_fb, &coords_fb);

    return attraction_fb;
}

AttractionItem AttractionsConverter::convert_from_fb(
    const AttractionsData::Attraction* attraction) {
    return AttractionItem{AttractionInfo(attraction->info()),
                          Coordinates(attraction->coords()->latitude(),
                                      attraction->coords()->longitude())};
}
