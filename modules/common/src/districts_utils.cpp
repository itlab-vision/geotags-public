// Copyright 2025 itlab-vision

#include <vector>

#include "districts_utils.hpp"  // NOLINT

namespace {

TgRingPtr deserialize_ring(const DistrictsData::Ring* fb_ring) {
    const auto* fb_pts = fb_ring->points();
    const int n = fb_pts->size();

    return TgRingPtr(
        tg_ring_new(reinterpret_cast<const tg_point*>(fb_pts->data()), n));
}

TgPolyPtr deserialize_polygon_raw(const DistrictsData::Polygon* fb_poly) {
    TgRingPtr ext = deserialize_ring(fb_poly->exterior());

    const auto* fb_holes = fb_poly->holes();
    std::vector<TgRingPtr> holes;
    std::vector<tg_ring*> raw_holes;
    holes.reserve(fb_holes ? fb_holes->size() : 0);
    raw_holes.reserve(fb_holes ? fb_holes->size() : 0);
    if (fb_holes) {
        for (const auto* h : *fb_holes) {
            holes.emplace_back(deserialize_ring(h));
            raw_holes.push_back(holes.back().get());
        }
    }

    tg_poly* raw_poly =
        tg_poly_new(ext.get(), raw_holes.data(), raw_holes.size());
    return TgPolyPtr(raw_poly);
}

TgGeomPtr deserialize_multipolygon(const DistrictsData::MultiPolygon* fb_mp) {
    std::vector<TgPolyPtr> polys;
    std::vector<tg_poly*> raw_polys;
    polys.reserve(fb_mp->polys()->size());
    raw_polys.reserve(fb_mp->polys()->size());
    for (const auto* p : *fb_mp->polys()) {
        polys.emplace_back(deserialize_polygon_raw(p));
        raw_polys.push_back(polys.back().get());
    }

    tg_geom* raw_mp =
        tg_geom_new_multipolygon(raw_polys.data(), raw_polys.size());
    return TgGeomPtr(raw_mp);
}

TgGeomPtr deserialize_polygon(const DistrictsData::Polygon* fb_poly) {
    TgPolyPtr poly = deserialize_polygon_raw(fb_poly);
    tg_geom* raw_geom = tg_geom_new_polygon(poly.get());
    return TgGeomPtr(raw_geom);
}

}  // namespace

TgGeomPtr deserialize_geom(const DistrictsData::GeometryUnion geom_type,
                           const void* data) {
    if (geom_type == DistrictsData::GeometryUnion_Polygon) {
        return deserialize_polygon(
            static_cast<const DistrictsData::Polygon*>(data));
    }

    return deserialize_multipolygon(
        static_cast<const DistrictsData::MultiPolygon*>(data));
}
