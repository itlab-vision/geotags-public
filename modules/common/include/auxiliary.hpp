// Copyright 2025 itlab-vision
#pragma once

#include <memory>
#include <utility>

extern "C" {
#include "tg.h"  // NOLINT
}

template <class T>
using Ptr = std::unique_ptr<T>;

template <class T, class V>
using Pair = std::pair<T, V>;

constexpr auto PI = 3.14159265359;
constexpr double DEG_TO_RAD = PI / 180.0;
constexpr auto EARTH_RADIUS = 6371.0;

enum class PrecisionState { Exact = 0, Approximate = 1 };

struct TgGeomDeleter {
    template <typename T>
    void operator()(T* g) const noexcept {
        if (g) {
            tg_geom_free(reinterpret_cast<tg_geom*>(g));
        }
    }
};

using TgGeomPtr = std::unique_ptr<tg_geom, TgGeomDeleter>;
using TgPolyPtr = std::unique_ptr<tg_poly, TgGeomDeleter>;
using TgRingPtr = std::unique_ptr<tg_ring, TgGeomDeleter>;

using TgGeomSharedPtr = std::shared_ptr<tg_geom>;
