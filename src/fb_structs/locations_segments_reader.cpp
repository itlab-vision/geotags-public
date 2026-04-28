// Copyright 2025 itlab-vision

#include <sys/stat.h>
#include <algorithm>
#include <utility>
#include <string>
#include <vector>
#include <sstream>

#include "locations_segments_reader.hpp"  // NOLINT

LocationsSegmentsReader::LocationsSegmentsReader(const std::string& fname)
    : Reader(fname) {
    open_file();
}

const flatbuffers::Vector<
    flatbuffers::Offset<LocationsSegmentsData::LocationsSegmentInfo>>*
LocationsSegmentsReader::deserialize() {
    std::string fb_file = file_name + ".fb";
    std::ifstream fin(fb_file, std::ios::binary | std::ios::ate);
    if (!fin) {
        throw std::runtime_error("Can't open FlatBuffer file: " + fb_file);
    }

    std::streamsize size = fin.tellg();
    fin.seekg(0, std::ios::beg);
    char* buffer = new char[size];  // FIXME memory leak
    if (!fin.read(buffer, size))
        throw std::runtime_error("Error reading FlatBuffer file");

    return LocationsSegmentsData::GetLocationsSegments(buffer)->data();
}

void LocationsSegmentsReader::serialize() {
    std::string segments_size;
    std::getline(file, segments_size);

    std::string attributes;
    std::getline(file, attributes);

    flatbuffers::FlatBufferBuilder builder(1024 * 1024);
    std::vector<
        flatbuffers::Offset<LocationsSegmentsData::LocationsSegmentInfo>>
        fb_segments;
    fb_segments.reserve(convert_num_argument<size_t>(segments_size));

    while (!file.eof()) {
        LocationsSegmentInfo segment;
        if (parse_line(segment)) {
            auto segment_id = segment.segment_id;
            auto lat_range = LocationsSegmentsData::LatitudeRange(
                segment.lat_range.first, segment.lat_range.second);
            auto lon_range = LocationsSegmentsData::LongitudeRange(
                segment.lon_range.first, segment.lon_range.second);

            auto neighbors_vector = builder.CreateVector(segment.neighbors);
            auto locations_vector = builder.CreateVector(segment.locations);

            auto segment_info_fb =
                LocationsSegmentsData::CreateLocationsSegmentInfo(
                    builder, segment_id, &lat_range, &lon_range,
                    neighbors_vector, locations_vector);
            fb_segments.push_back(segment_info_fb);
        }
    }
    auto data_vec = builder.CreateVector(fb_segments);
    auto segments_fb =
        LocationsSegmentsData::CreateLocationsSegments(builder, data_vec);
    builder.Finish(segments_fb);

    std::string fb_file = file_name + ".fb";
    std::ofstream fout(fb_file, std::ios::binary);
    fout.write(reinterpret_cast<const char*>(builder.GetBufferPointer()),
               builder.GetSize());
}

const flatbuffers::Vector<
    flatbuffers::Offset<LocationsSegmentsData::LocationsSegmentInfo>>*
LocationsSegmentsReader::read() {
    std::string fb_file = file_name + ".fb";
    if (!file_exists(fb_file) || file_mtime(file_name) > file_mtime(fb_file)) {
        serialize();
    }
    return deserialize();
}
