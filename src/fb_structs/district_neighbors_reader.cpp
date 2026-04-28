// Copyright 2025 itlab-vision

#include <sys/stat.h>
#include <fstream>
#include <string>
#include <utility>
#include <vector>
#include <unordered_map>

#include "district_info.hpp"                 // NOLINT
#include "district_neighbors_reader.hpp"     // NOLINT
#include "district_neighbors_generated.hpp"  // NOLINT
#include "district_reader.hpp"               // NOLINT
#include "find_district_neighbors.hpp"       // NOLINT

const flatbuffers::Vector<flatbuffers::Offset<DistrictNeighborsData::District>>*
DistrictNeighborsReader::deserialize() {
    std::ifstream fin(fb_file_name, std::ios::binary | std::ios::ate);
    if (!fin) {
        throw std::runtime_error("Can't open FlatBuffer file: " + fb_file_name);
    }

    std::streamsize size = fin.tellg();
    fin.seekg(0, std::ios::beg);
    char* buffer = new char[size];  // FIXME memory leak
    if (!fin.read(buffer, size))
        throw std::runtime_error("Error reading FlatBuffer file");

    return DistrictNeighborsData::GetDistrictNeighbors(buffer)->data();
}

void DistrictNeighborsReader::serialize() {
    // [1] Read districts from file (.csv)
    CSVDistrictsReader reader(file_name);
    std::vector<std::pair<DistrictInfo, struct tg_geom*>> districts =
        reader.read();

    // [2] Find neighbors for every district
    std::unordered_map<unsigned int, std::vector<unsigned int>> neighbors_map =
        find_district_neighbors(districts);
    // [3] Serialize to flatbuffer scheme
    flatbuffers::FlatBufferBuilder builder(1024 * 1024);
    std::vector<flatbuffers::Offset<DistrictNeighborsData::District>>
        fb_districts;

    for (const auto& pair : neighbors_map) {
        auto neighbors_vector = builder.CreateVector(pair.second);
        auto fb_district = DistrictNeighborsData::CreateDistrict(
            builder, pair.first, neighbors_vector);

        fb_districts.push_back(fb_district);
    }
    auto data_map = builder.CreateVectorOfSortedTables(&fb_districts);
    auto neighbors_fb =
        DistrictNeighborsData::CreateDistrictNeighbors(builder, data_map);
    builder.Finish(neighbors_fb);

    // [4] Write to .fb file
    std::ofstream fout(fb_file_name, std::ios::binary);
    fout.write(reinterpret_cast<const char*>(builder.GetBufferPointer()),
               builder.GetSize());

    // Free allocated space
    for (auto district : districts) {
        tg_geom_free(district.second);
    }
}

DistrictNeighborsReader::DistrictNeighborsReader(const std::string& path)
    : Reader(path) {
    open_file();
}

const flatbuffers::Vector<flatbuffers::Offset<DistrictNeighborsData::District>>*
DistrictNeighborsReader::read() {
    fb_file_name =
        file_name.substr(0, file_name.size() - 4) + "_neighbors.csv.fb";
    if (!file_exists(fb_file_name) ||
        file_mtime(file_name) > file_mtime(fb_file_name)) {
        serialize();
    }
    return deserialize();
}
