// Copyright 2025 itlab-vision
#pragma once

#include <variant>

#include "auxiliary.hpp"            // NOLINT
#include "districts_parser.hpp"     // NOLINT
#include "districts_converter.hpp"  // NOLINT
#include "generic_reader_fb.hpp"    // NOLINT

// Define wrapper type
#ifndef USE_FLATBUFFERS_STRUCTURES
#include "generic_reader_stl.hpp"     // NOLINT
#include "districts_wrapper_stl.hpp"  // NOLINT
using DistrictsWrapperType = DistrictsWrapperSTL;
using DistrictsReaderSTL =
    GenericReaderSTL<DistrictsParser, DistrictItem, DistrictsWrapperType>;
#else
#include "districts_wrapper_fb.hpp"  // NOLINT
using DistrictsWrapperType = DistrictsWrapperFB;
#endif

// Define reader types
using DistrictsReaderFB =
    GenericReaderFB<DistrictsParser, DistrictsConverter, DistrictsWrapperType>;

// Define the return Variant type
#ifndef USE_FLATBUFFERS_STRUCTURES
using DistrictsReaderPtr =
    std::variant<Ptr<DistrictsReaderSTL>, Ptr<DistrictsReaderFB>>;
#else
using DistrictsReaderPtr = std::variant<Ptr<DistrictsReaderFB>>;
#endif
