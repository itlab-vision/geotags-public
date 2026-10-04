// Copyright 2025 itlab-vision

#include <string>
#include <vector>

#include "distance.hpp"     // NOLINT
#include "coordinates.hpp"  // NOLINT

Ptr<Distance> Distance::create(const std::string& dist_formula) {
    if (dist_formula == "haversine") {
        return Ptr<Distance>(new HaversineDistance());
    } else if (dist_formula == "haversine_approx") {
        return Ptr<Distance>(new HaversineApproxDistance());
    } else {
        throw std::runtime_error("Unsupported distance type.");
    }
}

double HaversineDistance::calculate(double lat1, double lon1, double lat2,
                                    double lon2) const {
    Coordinates cr1Rad(convert_to_radians(lat1), convert_to_radians(lon1));
    Coordinates cr2Rad(convert_to_radians(lat2), convert_to_radians(lon2));
    double dlat = cr2Rad.latitude - cr1Rad.latitude;
    double dlon = cr2Rad.longitude - cr1Rad.longitude;
    double tmp = sin(dlat / 2.0) * sin(dlat / 2.0) +
                 cos(cr1Rad.latitude) * cos(cr2Rad.latitude) * sin(dlon / 2.0) *
                     sin(dlon / 2.0);
    return EARTH_RADIUS * 2.0 * atan2(sqrt(tmp), sqrt(1 - tmp));
}

double HaversineApproxDistance::calculate(double lat1, double lon1, double lat2,
                                          double lon2) const {
    Coordinates cr1Rad(convert_to_radians(lat1), convert_to_radians(lon1));
    Coordinates cr2Rad(convert_to_radians(lat2), convert_to_radians(lon2));
    double x = (cr2Rad.longitude - cr1Rad.longitude) *
               cos(0.5 * (cr2Rad.latitude + cr1Rad.latitude));
    double y = cr2Rad.latitude - cr1Rad.latitude;
    return EARTH_RADIUS * sqrt(x * x + y * y);
}
