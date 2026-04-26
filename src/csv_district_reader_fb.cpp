// Copyright 2025 itlab-vision
#include <sys/stat.h>
#include <algorithm>
#include <limits>
#include <string>
#include <utility>
#include <vector>

#include "districts_generated.hpp"  // NOLINT
#include "district_reader.hpp"      // NOLINT

CSVDistrictsReaderFB::CSVDistrictsReaderFB(const std::string &csv_path)
    : DistrictsReader(csv_path),
      DistrictsReaderFB(csv_path),
      CSVDistrictsReader(csv_path) {
    open_file();
}

std::vector<std::pair<DistrictInfo, tg_geom *>> CSVDistrictsReaderFB::read() {
    std::string fb_file = file_name + ".fb";
    if (!file_exists(fb_file) || file_mtime(file_name) > file_mtime(fb_file)) {
        serialize();
    }
    return deserialize();
}

std::vector<std::pair<DistrictInfo, tg_geom *>>
CSVDistrictsReaderFB::deserialize() {
    const std::string fb_path = file_name + ".fb";
    std::ifstream fin(fb_path, std::ios::binary | std::ios::ate);
    if (!fin) {
        throw std::runtime_error("Can't open FlatBuffer file: " + fb_path);
    }

    std::streamsize size = fin.tellg();
    fin.seekg(0, std::ios::beg);
    if (size <= 0) {
        throw std::runtime_error("Empty FlatBuffer file: " + fb_path);
    }

    std::vector<uint8_t> buf(static_cast<size_t>(size));
    if (!fin.read(reinterpret_cast<char *>(buf.data()), size)) {
        throw std::runtime_error("Error reading FlatBuffer file: " + fb_path);
    }

    flatbuffers::Verifier verifier(buf.data(), buf.size());
    if (!DistrictsData::VerifyDistrictsBuffer(verifier)) {
        throw std::runtime_error("Invalid FlatBuffer: " + fb_path);
    }

    const DistrictsData::Districts *root =
        DistrictsData::GetDistricts(buf.data());
    if (!root) {
        throw std::runtime_error("GetDistricts returned null");
    }

    const auto *entries = root->entries();
    if (!entries) {
        return {};
    }

    std::vector<std::pair<DistrictInfo, tg_geom *>> result;
    result.reserve(entries->size());

    for (size_t i = 0; i < entries->size(); ++i) {
        const auto *ent = entries->Get(i);
        if (!ent) {
            continue;
        }
        const auto *info_fb = ent->info();
        if (!info_fb) {
            continue;
        }

        DistrictInfo info{
            info_fb->id(),
            info_fb->name_en() ? info_fb->name_en()->str() : std::string(),
            info_fb->name() ? info_fb->name()->str() : std::string()};

        tg_geom *geom = deserialize_geom(ent->geom());
        result.emplace_back(info, geom);
    }

    return result;
}

tg_geom *CSVDistrictsReaderFB::deserialize_geom(
    const DistrictsData::Geometry *g) {
    if (!g) {
        return nullptr;
    }
    switch (g->gtype()) {
        case DistrictsData::GeometryType::GeometryType_Polygon:
            return deserialize_polygon(g->geom_as_Polygon());
        case DistrictsData::GeometryType::GeometryType_MultiPolygon:
            return deserialize_multipolygon(g->geom_as_MultiPolygon());
        default:
            return nullptr;
    }
}

tg_geom *CSVDistrictsReaderFB::deserialize_polygon(
    const DistrictsData::Polygon *fb_poly) {
    if (!fb_poly) {
        return nullptr;
    }
    const DistrictsData::Ring *ext = fb_poly->exterior();
    if (!ext) {
        return nullptr;
    }
    const auto *fb_pts = ext->points();
    if (!fb_pts) {
        return nullptr;
    }

    int n = static_cast<int>(fb_pts->size());
    if (n <= 0) {
        return nullptr;
    }

    std::vector<tg_point> pts;
    pts.reserve(n);
    for (int i = 0; i < n; ++i) {
        const DistrictsData::Point *p = fb_pts->Get(i);
        if (!p) {
            return nullptr;
        }
        pts.push_back({p->x(), p->y()});
    }

    tg_ring *ext_ring = tg_ring_new(pts.data(), n);
    if (!ext_ring) {
        return nullptr;
    }
    tg_poly *poly = tg_poly_new(ext_ring, nullptr, 0);
    if (!poly) {
        tg_geom_free(reinterpret_cast<tg_geom *>(ext_ring));
        return nullptr;
    }

    tg_geom *geom = tg_geom_new_polygon(poly);
    if (!geom) {
        tg_geom_free(reinterpret_cast<tg_geom *>(poly));
    }
    return geom;
}

tg_geom *CSVDistrictsReaderFB::deserialize_multipolygon(
    const DistrictsData::MultiPolygon *fb_mp) {
    if (!fb_mp) {
        return nullptr;
    }
    const auto *polys_fb = fb_mp->polys();
    if (!polys_fb) {
        return nullptr;
    }

    int np = static_cast<int>(polys_fb->size());
    if (np <= 0) {
        return nullptr;
    }

    std::vector<tg_poly *> polys;
    polys.reserve(np);

    for (int i = 0; i < np; ++i) {
        const DistrictsData::Polygon *pfb = polys_fb->Get(i);
        tg_geom *g = deserialize_polygon(pfb);
        if (!g) {
            for (tg_poly *pp : polys) {
                tg_geom_free(reinterpret_cast<tg_geom *>(pp));
            }
            return nullptr;
        }
        polys.push_back(const_cast<tg_poly *>(tg_geom_poly(g)));
    }

    tg_geom *mp =
        tg_geom_new_multipolygon(polys.data(), static_cast<int>(polys.size()));
    if (!mp) {
        for (tg_poly *pp : polys) {
            tg_geom_free(reinterpret_cast<tg_geom *>(pp));
        }
        return nullptr;
    }
    return mp;
}
