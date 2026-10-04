// Copyright 2025 itlab-vision
#pragma once

#include <variant>

#include "auxiliary.hpp"              // NOLINT
#include "attractions_parser.hpp"     // NOLINT
#include "attractions_converter.hpp"  // NOLINT
#include "generic_reader_fb.hpp"      // NOLINT

// Define wrapper type
#ifndef USE_FLATBUFFERS_STRUCTURES
#include "generic_reader_stl.hpp"  // NOLINT
#include "points_wrapper_stl.hpp"  // NOLINT
using AttractionsWrapperType = PointsWrapperSTL<AttractionItem>;
using AttractionsReaderSTL =
    GenericReaderSTL<AttractionsParser, AttractionItem, AttractionsWrapperType>;
#else
#include "points_wrapper_fb.hpp"  // NOLINT
using AttractionsWrapperType = PointsWrapperFB<AttractionsData::Attraction>;
#endif

// Define the FlatBuffers reader type
using AttractionsReaderFB =
    GenericReaderFB<AttractionsParser, AttractionsConverter,
                    AttractionsWrapperType>;

// Define the return Variant type
#ifndef USE_FLATBUFFERS_STRUCTURES
using AttractionsReaderPtr =
    std::variant<Ptr<AttractionsReaderSTL>, Ptr<AttractionsReaderFB>>;
#else
using AttractionsReaderPtr = std::variant<Ptr<AttractionsReaderFB>>;
#endif
