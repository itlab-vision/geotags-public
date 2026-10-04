// Copyright 2025 itlab-vision
#pragma once

#include <variant>

#include "auxiliary.hpp"            // NOLINT
#include "locations_parser.hpp"     // NOLINT
#include "locations_converter.hpp"  // NOLINT
#include "generic_reader_fb.hpp"    // NOLINT

// Define wrapper type
#ifndef USE_FLATBUFFERS_STRUCTURES
#include "generic_reader_stl.hpp"  // NOLINT
#include "points_wrapper_stl.hpp"  // NOLINT
using LocationsWrapperType = PointsWrapperSTL<LocationItem>;
using LocationsReaderSTL =
    GenericReaderSTL<LocationsParser, LocationItem, LocationsWrapperType>;
#else
#include "points_wrapper_fb.hpp"  // NOLINT
using LocationsWrapperType = PointsWrapperFB<LocationsData::Location>;
#endif

// Define reader types
using LocationsReaderFB =
    GenericReaderFB<LocationsParser, LocationsConverter, LocationsWrapperType>;

// Define the return Variant type
#ifndef USE_FLATBUFFERS_STRUCTURES
using LocationsReaderPtr =
    std::variant<Ptr<LocationsReaderSTL>, Ptr<LocationsReaderFB>>;
#else
using LocationsReaderPtr = std::variant<Ptr<LocationsReaderFB>>;
#endif
