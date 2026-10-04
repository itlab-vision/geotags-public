// Copyright 2025 itlab-vision
#pragma once

#include <iostream>
#include <string>
#include <unordered_map>

#include "coordinates.hpp"            // NOLINT
#include "attractions_generated.hpp"  // NOLINT

struct AttractionInfo {
    std::string type;
    std::string name;
    std::unordered_map<std::string, std::string> extra_fields;

    AttractionInfo() = default;

    AttractionInfo(const std::string& _type, const std::string& _name)
        : type(_type), name(_name) {}

    AttractionInfo(const std::string& _type, const std::string& _name,
                   const std::unordered_map<std::string, std::string>& fields)
        : type(_type), name(_name), extra_fields(fields) {}

    friend std::ostream& operator<<(std::ostream& out,
                                    const AttractionInfo& attraction) {
        out << "\tName: " << attraction.name << "\n";
        out << "\tType: " << attraction.type << "\n";

        if (!attraction.extra_fields.empty()) {
            out << "\tExtra fields:\n";
            for (const auto& [key, value] : attraction.extra_fields) {
                out << "\t\t" << key << ": " << value << "\n";
            }
        }

        return out;
    }

    AttractionInfo& operator=(const AttractionsData::AttractionInfo* info) {
        this->type = info->type()->str();
        this->name = info->name()->str();

        this->extra_fields.clear();
        auto fb_extra_fields = info->extra_fields();
        if (fb_extra_fields) {
            this->extra_fields.reserve(fb_extra_fields->size());
            for (auto fb_field : *fb_extra_fields) {
                this->extra_fields.emplace(fb_field->key()->str(),
                                           fb_field->value()->str());
            }
        }

        return *this;
    }

    explicit AttractionInfo(const AttractionsData::AttractionInfo* info) {
        *this = info;
    }
};
