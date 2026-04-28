// Copyright 2025 itlab-vision
#pragma once

#include <fstream>
#include <string>
#include <utility>
#include <vector>
#include "reader.hpp"  // NOLINT

extern "C" {
#include "tg.h"  // NOLINT
}

#include "coordinates.hpp"          // NOLINT
#include "district_info.hpp"        // NOLINT
#include "auxiliary.hpp"            // NOLINT
#include "districts_generated.hpp"  // NOLINT

class DistrictsReader : public Reader {
 public:
    explicit DistrictsReader(const std::string &fname) : Reader(fname) {}
    virtual std::vector<std::pair<DistrictInfo, tg_geom *>> read() = 0;
    static Ptr<DistrictsReader> create(const std::string &reader_type,
                                       const std::string &file_name);
    virtual ~DistrictsReader() = default;
};

class CSVDistrictsReader : virtual public DistrictsReader {
 protected:
    bool parse_line(std::pair<DistrictInfo, tg_geom *> &location);

 public:
    explicit CSVDistrictsReader(const std::string &csv_path);
    std::vector<std::pair<DistrictInfo, tg_geom *>> read();
};

class DistrictsReaderFB : virtual public DistrictsReader {
 protected:
    virtual std::vector<std::pair<DistrictInfo, tg_geom *>> deserialize() = 0;
    virtual void serialize() = 0;

 public:
    explicit DistrictsReaderFB(const std::string &fname)
        : DistrictsReader(fname) {}
};

class CSVDistrictsReaderFB : public DistrictsReaderFB,
                             public CSVDistrictsReader {
 private:
    void serialize();
    static flatbuffers::Offset<DistrictsData::Geometry> serialize_geom(
        flatbuffers::FlatBufferBuilder &b, const tg_geom *geom);

    static flatbuffers::Offset<DistrictsData::Ring> serialize_ring(
        flatbuffers::FlatBufferBuilder &builder, const tg_ring *ring);

    static flatbuffers::Offset<DistrictsData::Polygon> serialize_polygon(
        flatbuffers::FlatBufferBuilder &builder, const tg_poly *poly);

    static flatbuffers::Offset<DistrictsData::MultiPolygon>
    serialize_multipolygon(flatbuffers::FlatBufferBuilder &builder,
                           const tg_geom *geom);

    std::vector<std::pair<DistrictInfo, tg_geom *>> deserialize();
    static tg_geom *deserialize_geom(const DistrictsData::Geometry *g);
    static tg_geom *deserialize_polygon(const DistrictsData::Polygon *p);
    static tg_geom *deserialize_multipolygon(
        const DistrictsData::MultiPolygon *mp);

 public:
    explicit CSVDistrictsReaderFB(const std::string &csv_path);
    std::vector<std::pair<DistrictInfo, tg_geom *>> read();
};
