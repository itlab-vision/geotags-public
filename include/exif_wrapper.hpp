// Copyright 2025 itlab-vision
#pragma once

#include <string>

#include "TinyEXIF.h"

class EXIFData {
 private:
    TinyEXIF::EXIFInfo imageEXIF;
    double lat, lon;
    std::string datetime_original;
    std::string datetime;
    void parse_coordinates();
    void parse_datetime();

 public:
    explicit EXIFData(const std::string& image_path);

    double get_lat() const { return lat; }
    double get_lon() const { return lon; }
    std::string get_datetime() const { return datetime; }
    std::string get_datetime_original() const { return datetime_original; }
};
