// Copyright 2025 itlab-vision

#include <vector>
#include <utility>

#include "districts_converter.hpp"  // NOLINT

flatbuffers::Offset<DistrictsData::DistrictEntry>
DistrictsConverter::serialize_district(flatbuffers::FlatBufferBuilder& builder,
                                       const DistrictInfo& info,
                                       const tg_geom* geom) {
    auto info_off = serialize_info(builder, info);
    auto geom_off = serialize_geom(builder, geom);

    DistrictsData::DistrictEntryBuilder eb(builder);
    eb.add_info(info_off);
    if (tg_geom_typeof(geom) == TG_POLYGON) {
        eb.add_geom_type(DistrictsData::GeometryUnion_Polygon);
    } else {
        eb.add_geom_type(DistrictsData::GeometryUnion_MultiPolygon);
    }
    eb.add_geom(geom_off);
    return eb.Finish();
}

flatbuffers::Offset<DistrictsData::DistrictInfo>
DistrictsConverter::serialize_info(flatbuffers::FlatBufferBuilder& builder,
                                   const DistrictInfo& info) {
    auto name_en = builder.CreateString(info.name_en);
    auto name = builder.CreateString(info.name);

    DistrictsData::DistrictInfoBuilder ib(builder);
    ib.add_id(info.id);
    ib.add_name_en(name_en);
    ib.add_name(name);

    return ib.Finish();
}

flatbuffers::Offset<void> DistrictsConverter::serialize_geom(
    flatbuffers::FlatBufferBuilder& builder, const tg_geom* geom) {
    if (tg_geom_typeof(geom) == TG_POLYGON) {
        return serialize_polygon(builder, tg_geom_poly(geom)).Union();
    }
    return serialize_multipolygon(builder, geom).Union();
}

flatbuffers::Offset<DistrictsData::Ring> DistrictsConverter::serialize_ring(
    flatbuffers::FlatBufferBuilder& builder, const tg_ring* ring) {
    int n = tg_ring_num_points(ring);
    const tg_point* pts = tg_ring_points(ring);

    std::vector<DistrictsData::Point> fb_pts(n);
    memcpy(fb_pts.data(), pts, n * sizeof(DistrictsData::Point));

    auto points_off =
        builder.CreateVectorOfStructs<DistrictsData::Point>(fb_pts.data(), n);

    DistrictsData::RingBuilder rb(builder);
    rb.add_points(points_off);

    return rb.Finish();
}

flatbuffers::Offset<DistrictsData::Polygon>
DistrictsConverter::serialize_polygon(flatbuffers::FlatBufferBuilder& builder,
                                      const tg_poly* poly) {
    auto ext_off = serialize_ring(builder, tg_poly_exterior(poly));

    int nh = tg_poly_num_holes(poly);
    std::vector<flatbuffers::Offset<DistrictsData::Ring>> holes;
    holes.reserve(nh);

    for (int i = 0; i < nh; ++i) {
        holes.push_back(serialize_ring(builder, tg_poly_hole_at(poly, i)));
    }

    auto holes_vec = builder.CreateVector(holes);

    DistrictsData::PolygonBuilder pb(builder);
    pb.add_exterior(ext_off);
    if (nh > 0) pb.add_holes(holes_vec);

    return pb.Finish();
}

flatbuffers::Offset<DistrictsData::MultiPolygon>
DistrictsConverter::serialize_multipolygon(
    flatbuffers::FlatBufferBuilder& builder, const tg_geom* geom) {
    int np = tg_geom_num_polys(geom);

    std::vector<flatbuffers::Offset<DistrictsData::Polygon>> polys;
    polys.reserve(np);

    for (int i = 0; i < np; ++i) {
        polys.push_back(serialize_polygon(builder, tg_geom_poly_at(geom, i)));
    }

    auto polys_vec = builder.CreateVector(polys);

    DistrictsData::MultiPolygonBuilder mp(builder);
    mp.add_polys(polys_vec);

    return mp.Finish();
}

flatbuffers::Offset<DistrictsData::DistrictEntry>
DistrictsConverter::convert_to_fb(flatbuffers::FlatBufferBuilder& builder,
                                  const DistrictItem& district) {
    const DistrictInfo& info = district.first;
    const TgGeomSharedPtr& geom = district.second;
    return serialize_district(builder, info, geom.get());
}

DistrictItem DistrictsConverter::convert_from_fb(
    const DistrictsData::DistrictEntry* district_entry) {
    return DistrictItem{
        DistrictInfo(district_entry->info()),
        deserialize_geom(district_entry->geom_type(), district_entry->geom())};
}
