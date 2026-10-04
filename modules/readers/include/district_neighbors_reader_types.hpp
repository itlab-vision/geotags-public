// Copyright 2025 itlab-vision
#pragma once

#include <variant>

#include "auxiliary.hpp"                     // NOLINT
#include "district_neighbors_parser.hpp"     // NOLINT
#include "district_neighbors_converter.hpp"  // NOLINT
#include "generic_reader_fb.hpp"             // NOLINT

// Define wrapper type
#ifndef USE_FLATBUFFERS_STRUCTURES
#include "generic_reader_stl.hpp"              // NOLINT
#include "district_neighbors_wrapper_stl.hpp"  // NOLINT
using DistrictNeighborsWrapperType = DistrictNeighborsWrapperSTL;
using DistrictNeighborsReaderSTL =
    GenericReaderSTL<DistrictNeighborsParser, DistrictNeighborInfo,
                     DistrictNeighborsWrapperType>;
#else
#include "district_neighbors_wrapper_fb.hpp"  // NOLINT
using DistrictNeighborsWrapperType = DistrictNeighborsWrapperFB;
#endif

// Define reader types
using DistrictNeighborsReaderFB =
    GenericReaderFB<DistrictNeighborsParser, DistrictNeighborsConverter,
                    DistrictNeighborsWrapperType>;

// Define the return Variant type
#ifndef USE_FLATBUFFERS_STRUCTURES
using DistrictNeighborsReaderPtr = std::variant<Ptr<DistrictNeighborsReaderSTL>,
                                                Ptr<DistrictNeighborsReaderFB>>;
#else
using DistrictNeighborsReaderPtr = std::variant<Ptr<DistrictNeighborsReaderFB>>;
#endif
