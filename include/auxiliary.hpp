// Copyright 2025 itlab-vision
#pragma once

#include <memory>
#include <utility>

template <class T>
using Ptr = std::unique_ptr<T>;

template <class T, class V>
using Pair = std::pair<T, V>;

constexpr auto PI = 3.14159265359;
constexpr auto EARTH_RADIUS = 6371.0;

enum class NearestLocationState {
    ExactLocation = 0,
    ApproximateLocation = 1,
};
