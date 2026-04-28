// Copyright 2025 itlab-vision
#include "exif_wrapper.hpp"  // NOLINT

#include <fstream>
#include <limits>
#include <stdexcept>
#include <string>
#include <tuple>

EXIFData::EXIFData(const std::string &image_path) {
    std::ifstream stream(image_path, std::ios::binary);
    if (!stream) {
        throw std::invalid_argument("Can't open file: " + image_path);
    }
    imageEXIF = TinyEXIF::EXIFInfo(stream);
    parse_coordinates();
    parse_datetime();
}

void EXIFData::parse_coordinates() {
    if (imageEXIF.GeoLocation.hasLatLon()) {
        lat = imageEXIF.GeoLocation.Latitude;
        lon = imageEXIF.GeoLocation.Longitude;
    } else {
        throw std::invalid_argument("Image doesn't have metadata.");
    }
}

void EXIFData::parse_datetime() {
    if (!imageEXIF.DateTime.empty())
        datetime = imageEXIF.DateTime;
    else
        datetime = "No datetime";
    if (!imageEXIF.DateTimeOriginal.empty())
        datetime_original = imageEXIF.DateTimeOriginal.c_str();
    else
        datetime_original = "No datetime";
}
