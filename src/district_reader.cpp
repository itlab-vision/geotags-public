// Copyright 2025 itlab-vision

#include <sys/stat.h>

#include <algorithm>
#include <utility>
#include <string>
#include <vector>
#include <sstream>

#include "district_reader.hpp"      // NOLINT
#include "district_info.hpp"        // NOLINT
#include "auxiliary.hpp"            // NOLINT
#include "districts_generated.hpp"  // NOLINT

extern "C" {
#include "tg.h"  // NOLINT
}

Ptr<DistrictsReader> DistrictsReader::create(const std::string &reader_type,
                                             const std::string &file_name) {
    if (reader_type == "csv") {
        return Ptr<DistrictsReader>(new CSVDistrictsReader(file_name));
    } else if (reader_type == "csv_fb") {
        return Ptr<DistrictsReader>(new CSVDistrictsReaderFB(file_name));
    } else {
        throw std::runtime_error("Unsupported reader type");
    }
}

void CSVDistrictsReaderFB::serialize() {
    std::string header;
    std::getline(file, header);

    flatbuffers::FlatBufferBuilder builder(1024 * 1024);
    std::vector<flatbuffers::Offset<DistrictsData::DistrictEntry>> entries;
    entries.reserve(1024);

    std::pair<DistrictInfo, tg_geom *> district;
    while (!file.eof()) {
        if (!parse_line(district)) continue;

        const DistrictInfo &info = district.first;
        tg_geom *geom = district.second;
        if (!geom) continue;

        // Info
        auto name_en_off = builder.CreateString(info.name_en);
        auto name_off = builder.CreateString(info.name);
        DistrictsData::DistrictInfoBuilder dib(builder);
        dib.add_id(info.id);
        dib.add_name_en(name_en_off);
        dib.add_name(name_off);
        auto info_off = dib.Finish();

        // Geometry
        auto geom_off = serialize_geom(builder, geom);

        DistrictsData::DistrictEntryBuilder reb(builder);
        reb.add_info(info_off);
        reb.add_geom(geom_off);
        entries.push_back(reb.Finish());

        tg_geom_free(geom);
    }

    auto entries_vec = builder.CreateVector(entries);
    auto root = DistrictsData::CreateDistricts(builder, entries_vec);
    builder.Finish(root);

    std::ofstream fout(file_name + ".fb", std::ios::binary);
    fout.write(reinterpret_cast<const char *>(builder.GetBufferPointer()),
               builder.GetSize());
}

flatbuffers::Offset<DistrictsData::Geometry>
CSVDistrictsReaderFB::serialize_geom(flatbuffers::FlatBufferBuilder &builder,
                                     const tg_geom *geom) {
    if (tg_geom_typeof(geom) == TG_POLYGON) {
        auto poly_off = serialize_polygon(builder, tg_geom_poly(geom));
        DistrictsData::GeometryBuilder gb(builder);
        gb.add_gtype(DistrictsData::GeometryType::GeometryType_Polygon);
        gb.add_geom_type(DistrictsData::GeometryUnion::GeometryUnion_Polygon);
        gb.add_geom(poly_off.Union());
        return gb.Finish();
    } else {
        auto mp_off = serialize_multipolygon(builder, geom);
        DistrictsData::GeometryBuilder gb(builder);
        gb.add_gtype(DistrictsData::GeometryType::GeometryType_MultiPolygon);
        gb.add_geom_type(
            DistrictsData::GeometryUnion::GeometryUnion_MultiPolygon);
        gb.add_geom(mp_off.Union());
        return gb.Finish();
    }
}

flatbuffers::Offset<DistrictsData::Ring> CSVDistrictsReaderFB::serialize_ring(
    flatbuffers::FlatBufferBuilder &builder, const tg_ring *ring) {
    int n = tg_ring_num_points(ring);
    const tg_point *pts = tg_ring_points(ring);

    std::vector<DistrictsData::Point> tmp;
    tmp.reserve(n);
    for (int i = 0; i < n; ++i) tmp.emplace_back(pts[i].x, pts[i].y);

    auto points_off = builder.CreateVectorOfStructs<DistrictsData::Point>(
        tmp.data(), tmp.size());

    struct tg_rect r = tg_ring_rect(ring);
    DistrictsData::Rect bbox(DistrictsData::Point(r.min.x, r.min.y),
                             DistrictsData::Point(r.max.x, r.max.y));

    DistrictsData::RingBuilder rb(builder);
    rb.add_points(points_off);
    rb.add_npoints(n);
    rb.add_area(tg_ring_area(ring));
    rb.add_bbox(&bbox);
    return rb.Finish();
}

flatbuffers::Offset<DistrictsData::Polygon>
CSVDistrictsReaderFB::serialize_polygon(flatbuffers::FlatBufferBuilder &builder,
                                        const tg_poly *poly) {
    auto ext_off = serialize_ring(builder, tg_poly_exterior(poly));

    int nh = tg_poly_num_holes(poly);
    std::vector<flatbuffers::Offset<DistrictsData::Ring>> holes;
    holes.reserve(nh);
    for (int i = 0; i < nh; ++i)
        holes.push_back(serialize_ring(builder, tg_poly_hole_at(poly, i)));

    auto holes_off = builder.CreateVector(holes);

    struct tg_rect r = tg_poly_rect(poly);
    DistrictsData::Rect bbox(DistrictsData::Point(r.min.x, r.min.y),
                             DistrictsData::Point(r.max.x, r.max.y));

    DistrictsData::PolygonBuilder pb(builder);
    pb.add_exterior(ext_off);
    if (!holes.empty()) pb.add_holes(holes_off);
    pb.add_nholes(nh);
    pb.add_bbox(&bbox);

    return pb.Finish();
}

flatbuffers::Offset<DistrictsData::MultiPolygon>
CSVDistrictsReaderFB::serialize_multipolygon(
    flatbuffers::FlatBufferBuilder &builder, const tg_geom *geom) {
    int np = tg_geom_num_polys(geom);
    std::vector<flatbuffers::Offset<DistrictsData::Polygon>> polys;
    polys.reserve(np);
    for (int i = 0; i < np; ++i)
        polys.push_back(serialize_polygon(builder, tg_geom_poly_at(geom, i)));

    auto polys_off = builder.CreateVector(polys);

    struct tg_rect r = tg_geom_rect(geom);
    DistrictsData::Rect bbox(DistrictsData::Point(r.min.x, r.min.y),
                             DistrictsData::Point(r.max.x, r.max.y));

    DistrictsData::MultiPolygonBuilder mp(builder);
    mp.add_polys(polys_off);
    mp.add_npolys(np);
    mp.add_bbox(&bbox);

    return mp.Finish();
}
