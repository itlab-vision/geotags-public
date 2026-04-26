// Copyright 2025 itlab-vision

#include "not_fb_structs/locations_reader.hpp"  // NOLINT

#include <sys/stat.h>
#include <string>

Ptr<LocationsReader> LocationsReader::create(const std::string &reader_type,
                                             const std::string &file_name) {
    if (reader_type == "csv") {
        return Ptr<LocationsReader>(new CSVLocationsReader(file_name));
    } else if (reader_type == "csv_fb") {
        return Ptr<LocationsReader>(new CSVLocationsReaderFB(file_name));
    } else {
        throw std::runtime_error("Unsupported reader type");
    }
}
