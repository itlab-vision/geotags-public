// Copyright 2025 itlab-vision
#pragma once

#include <cmath>

#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "auxiliary.hpp"      // NOLINT
#include "location_info.hpp"  // NOLINT

class Distance {
 protected:
    static inline constexpr double convert_to_radians(double degree) {
        return degree * DEG_TO_RAD;
    }

 public:
    virtual ~Distance() = default;
    virtual double calculate(double lat1, double lon1, double lat2,
                             double lon2) const = 0;
    static Ptr<Distance> create(const std::string& dist_formula);
};

class HaversineDistance : public Distance {
 public:
    double calculate(double lat1, double lon1, double lat2,
                     double lon2) const override;
};

class HaversineApproxDistance : public Distance {
 public:
    double calculate(double lat1, double lon1, double lat2,
                     double lon2) const override;
};
