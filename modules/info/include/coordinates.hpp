// Copyright 2025 itlab-vision
#pragma once

struct Coordinates {
    double latitude;
    double longitude;

    Coordinates() : latitude(0.0), longitude(0.0) {}
    Coordinates(double lat, double lon) : latitude(lat), longitude(lon) {}
};
