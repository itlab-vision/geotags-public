// Copyright 2025 itlab-vision
#include <sys/stat.h>
#include <algorithm>
#include <limits>
#include <string>
#include <utility>
#include <vector>
#include <sstream>

#include "locations_generated.hpp"  // NOLINT
#include "locations_reader.hpp"     // NOLINT

CSVLocationsReaderFB::CSVLocationsReaderFB(const std::string &csv_path)
    : CSVLocationParser(csv_path), Reader(csv_path) {
    open_file();
}

const flatbuffers::Vector<flatbuffers::Offset<LocationsData::Location>> *
CSVLocationsReaderFB::deserialize() {
    std::string fb_file = file_name + ".fb";
    std::ifstream fin(fb_file, std::ios::binary | std::ios::ate);
    if (!fin) {
        throw std::runtime_error("Can't open FlatBuffer file: " + fb_file);
    }

    std::streamsize size = fin.tellg();
    fin.seekg(0, std::ios::beg);
    char *buffer = new char[size];  // FIXME memory leak
    if (!fin.read(buffer, size))
        throw std::runtime_error("Error reading FlatBuffer file");

    return LocationsData::GetLocations(buffer)->data();
}

const flatbuffers::Vector<flatbuffers::Offset<LocationsData::Location>> *
CSVLocationsReaderFB::read() {
    prepare_file();
    return deserialize();
}
