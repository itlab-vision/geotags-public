// Copyright 2025 itlab-vision
#pragma once

#include <variant>

#include "auxiliary.hpp"           // NOLINT
#include "segments_parser.hpp"     // NOLINT
#include "segments_converter.hpp"  // NOLINT
#include "generic_reader_fb.hpp"   // NOLINT

// Define wrapper type
#ifndef USE_FLATBUFFERS_STRUCTURES
#include "generic_reader_stl.hpp"    // NOLINT
#include "segments_wrapper_stl.hpp"  // NOLINT
using SegmentsWrapperType = SegmentsWrapperSTL;
using SegmentsReaderSTL =
    GenericReaderSTL<SegmentsParser, SegmentInfo, SegmentsWrapperType>;
#else
#include "segments_wrapper_fb.hpp"  // NOLINT
using SegmentsWrapperType = SegmentsWrapperFB;
#endif

// Define reader types
using SegmentsReaderFB =
    GenericReaderFB<SegmentsParser, SegmentsConverter, SegmentsWrapperType>;

// Define the return Variant type
#ifndef USE_FLATBUFFERS_STRUCTURES
using SegmentsReaderPtr =
    std::variant<Ptr<SegmentsReaderSTL>, Ptr<SegmentsReaderFB>>;
#else
using SegmentsReaderPtr = std::variant<Ptr<SegmentsReaderFB>>;
#endif
