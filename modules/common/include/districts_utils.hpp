// Copyright 2025 itlab-vision
#pragma once

#include <utility>
#include <vector>
#include <string>

#include "auxiliary.hpp"            // NOLINT
#include "districts_generated.hpp"  // NOLINT

extern "C" {
#include "tg.h"  // NOLINT
}

TgGeomPtr deserialize_geom(const DistrictsData::GeometryUnion geom_type,
                           const void* g);
