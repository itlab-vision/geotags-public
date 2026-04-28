// Copyright 2025 itlab-vision
#pragma once

#include <cmath>

#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "auxiliary.hpp"      // NOLINT
#include "location_info.hpp"  // NOLINT
#include "coordinates.hpp"    // NOLINT

class Distance {
 protected:
    double convert_to_radians(double degree) { return degree * PI / 180.0; }

 public:
    virtual double calculate(const Coordinates &cr1,
                             const Coordinates &cr2) = 0;
    static Ptr<Distance> create(const std::string &dist_formula);
};

class HaversineDistance : public Distance {
 public:
    double calculate(const Coordinates &cr1, const Coordinates &cr2);
};

class HaversineApproxDistance : public Distance {
 public:
    double calculate(const Coordinates &cr1, const Coordinates &cr2);
};
