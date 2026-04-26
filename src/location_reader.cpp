// Copyright 2025 itlab-vision
#include <limits>
#include <utility>
#include <vector>
#include <fstream>
#include <string>

#include "locations_generated.hpp"  // NOLINT
#include "locations_reader.hpp"     // NOLINT

void CSVLocationsReaderFB::serialize() {
    std::string locations_size;
    std::getline(file, locations_size);

    std::string attributes;
    std::getline(file, attributes);
    detect_parser_format(attributes);

    flatbuffers::FlatBufferBuilder builder(1024 * 1024);
    std::vector<flatbuffers::Offset<LocationsData::Location>> fb_locations;
    fb_locations.reserve(convert_num_argument<size_t>(locations_size));

    while (!file.eof()) {
        std::pair<LocationInfo, Coordinates> location = std::make_pair(
            LocationInfo(), Coordinates(std::numeric_limits<double>::max(),
                                        std::numeric_limits<double>::max()));
        if (parse_line(location)) {
            auto &info = location.first;
            auto &coords = location.second;

            auto location_id = info.location_id;
            auto country = builder.CreateString(info.country);
            auto city = builder.CreateString(info.city);
            auto region_id = info.region_id;
            auto district_id = info.district_id;
            std::vector<flatbuffers::Offset<flatbuffers::String>> alt_names_vec;
            for (const auto &name : info.alt_names)
                alt_names_vec.push_back(builder.CreateString(name));
            auto alt_names = builder.CreateVector(alt_names_vec);

            auto info_fb = LocationsData::CreateLocationInfo(
                builder, location_id, country, city, region_id, district_id,
                alt_names);
            auto coords_fb =
                LocationsData::Coordinates(coords.latitude, coords.longitude);
            auto loc_fb =
                LocationsData::CreateLocation(builder, info_fb, &coords_fb);
            fb_locations.push_back(loc_fb);
        }
    }
    auto data_vec = builder.CreateVector(fb_locations);
    auto locations_fb = LocationsData::CreateLocations(builder, data_vec);
    builder.Finish(locations_fb);

    std::string fb_file = file_name + ".fb";
    std::ofstream fout(fb_file, std::ios::binary);
    fout.write(reinterpret_cast<const char *>(builder.GetBufferPointer()),
               builder.GetSize());
}

void CSVLocationsReaderFB::prepare_file() {
    std::string fb_file = file_name + ".fb";
    if (!file_exists(fb_file) || file_mtime(file_name) > file_mtime(fb_file)) {
        serialize();
    }
}
