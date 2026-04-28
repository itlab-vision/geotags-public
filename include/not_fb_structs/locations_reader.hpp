// Copyright 2025 itlab-vision
#pragma once

#include <fstream>
#include <string>
#include <utility>
#include <vector>

#include "coordinates.hpp"           // NOLINT
#include "location_info.hpp"         // NOLINT
#include "auxiliary.hpp"             // NOLINT
#include "reader.hpp"                // NOLINT
#include "csv_locations_parser.hpp"  // NOLINT

class LocationsReader : virtual public Reader {
 public:
    explicit LocationsReader(const std::string &fname) : Reader(fname) {}
    virtual std::vector<std::pair<LocationInfo, Coordinates>> read() = 0;
    static Ptr<LocationsReader> create(const std::string &reader_type,
                                       const std::string &file_name);
    virtual ~LocationsReader() = default;
};

class CSVLocationsReader : virtual public LocationsReader,
                           public CSVLocationParser {
 public:
    explicit CSVLocationsReader(const std::string &csv_path);
    std::vector<std::pair<LocationInfo, Coordinates>> read();
};

class LocationsReaderFB : virtual public LocationsReader {
 protected:
    virtual std::vector<std::pair<LocationInfo, Coordinates>> deserialize() = 0;
    virtual void serialize() = 0;

 public:
    explicit LocationsReaderFB(const std::string &fname)
        : LocationsReader(fname) {}
};

class CSVLocationsReaderFB : public LocationsReaderFB,
                             public CSVLocationsReader {
 private:
    void prepare_file();
    std::vector<std::pair<LocationInfo, Coordinates>> deserialize();
    void serialize();

 public:
    explicit CSVLocationsReaderFB(const std::string &csv_path);
    std::vector<std::pair<LocationInfo, Coordinates>> read();
};
