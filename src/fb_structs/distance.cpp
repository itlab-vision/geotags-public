// Copyright 2025 itlab-vision

#include <string>
#include <vector>

#include "distance.hpp"     // NOLINT
#include "coordinates.hpp"  // NOLINT

double HaversineDistance::calculate(const LocationsData::Coordinates *cr1,
                                    const Coordinates &cr2) {
    Coordinates cr1Rad(convert_to_radians(cr1->latitude()),
                       convert_to_radians(cr1->longitude()));
    Coordinates cr2Rad(convert_to_radians(cr2.latitude),
                       convert_to_radians(cr2.longitude));
    double dlat = cr2Rad.latitude - cr1Rad.latitude;
    double dlon = cr2Rad.longitude - cr1Rad.longitude;
    double tmp = sin(dlat / 2.0) * sin(dlat / 2.0) +
                 cos(cr1Rad.latitude) * cos(cr2Rad.latitude) * sin(dlon / 2.0) *
                     sin(dlon / 2.0);
    return EARTH_RADIUS * 2.0 * atan2(sqrt(tmp), sqrt(1 - tmp));
}

double HaversineApproxDistance::calculate(const LocationsData::Coordinates *cr1,
                                          const Coordinates &cr2) {
    Coordinates cr1Rad(convert_to_radians(cr1->latitude()),
                       convert_to_radians(cr1->longitude()));
    Coordinates cr2Rad(convert_to_radians(cr2.latitude),
                       convert_to_radians(cr2.longitude));
    double x = (cr2Rad.longitude - cr1Rad.longitude) *
               cos(0.5 * (cr2Rad.latitude + cr1Rad.latitude));
    double y = cr2Rad.latitude - cr1Rad.latitude;
    return EARTH_RADIUS * sqrt(x * x + y * y);
}
