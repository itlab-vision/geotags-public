// Copyright 2025 itlab-vision
#pragma once

#include <iostream>
#include <string>
#include <vector>

#include "districts_generated.hpp"  // NOLINT

struct DistrictInfo {
    unsigned int id;
    std::string name_en;
    std::string name;

    DistrictInfo() {}
    DistrictInfo(unsigned int _id, std::string ne, std::string n)
        : id(_id), name_en(ne), name(n) {}

    friend std::ostream& operator<<(std::ostream& out,
                                    const DistrictInfo& district) {
        out << "\tEN Name: " << district.name_en << "\n";
        out << "\tName: " << district.name << "\n";
        return out;
    }

    DistrictInfo& operator=(const DistrictsData::DistrictInfo* info) {
        this->id = info->id();
        this->name_en = info->name_en()->str();
        this->name = info->name()->str();

        return *this;
    }

    explicit DistrictInfo(const DistrictsData::DistrictInfo* info) {
        *this = info;
    }
};
