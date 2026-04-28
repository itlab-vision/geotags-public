// Copyright 2025 itlab-vision
#include <sys/stat.h>
#include <algorithm>
#include <limits>
#include <string>
#include <utility>
#include <vector>

#include "locations_generated.hpp"  // NOLINT
#include "locations_reader.hpp"     // NOLINT

CSVLocationsReaderFB::CSVLocationsReaderFB(const std::string &csv_path)
    : LocationsReader(csv_path),
      LocationsReaderFB(csv_path),
      CSVLocationsReader(csv_path),
      Reader(csv_path) {
    open_file();
}

std::vector<std::pair<LocationInfo, Coordinates>>
CSVLocationsReaderFB::deserialize() {
    std::string fb_file = file_name + ".fb";
    std::ifstream fin(fb_file, std::ios::binary | std::ios::ate);
    if (!fin) {
        throw std::runtime_error("Can't open FlatBuffer file: " + fb_file);
    }

    std::streamsize size = fin.tellg();
    fin.seekg(0, std::ios::beg);
    char *buffer = new char[size];  // FIXME memory leak
    if (!fin.read(buffer, size)) {
        throw std::runtime_error("Error reading FlatBuffer file");
    }

    const auto &locations_fb = LocationsData::GetLocations(buffer)->data();
    std::vector<std::pair<LocationInfo, Coordinates>> locations;
    locations.reserve(locations_fb->size());
    for (auto loc : *locations_fb) {
        std::vector<std::string> alt_names;
        auto fb_alt_names = loc->info()->alt_names();
        alt_names.reserve(fb_alt_names->size());
        if (fb_alt_names) {
            for (auto it = fb_alt_names->begin(); it != fb_alt_names->end();
                 ++it) {
                alt_names.push_back(it->str());
            }
        }
        LocationInfo info(loc->info()->location_id(),
                          loc->info()->country()->str(),
                          loc->info()->city()->str(), loc->info()->region_id(),
                          loc->info()->district_id(), alt_names);
        Coordinates coords(loc->coords()->latitude(),
                           loc->coords()->longitude());

        locations.emplace_back(info, coords);
    }

    return locations;
}

std::vector<std::pair<LocationInfo, Coordinates>> CSVLocationsReaderFB::read() {
    prepare_file();
    return deserialize();
}
