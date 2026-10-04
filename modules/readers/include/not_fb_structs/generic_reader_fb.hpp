// Copyright 2025 itlab-vision
#pragma once

#include <string>
#include <vector>
#include <utility>

#include "flatbuffers/flatbuffers.h"   // NOLINT
#include "base_generic_reader_fb.hpp"  // NOLINT

template <typename Parser, typename Converter, typename WrapperType>
class GenericReaderFB
    : public BaseGenericReaderFB<Parser, Converter, WrapperType> {
 public:
    explicit GenericReaderFB(const std::string& fname)
        : BaseGenericReaderFB<Parser, Converter, WrapperType>(fname) {}

 protected:
    WrapperType deserialize() override {
        auto buffer = this->read_buffer();

        // NOTE: Buffer verification can be added here.
        // This would require adding a verify_buffer() method to the base
        // ConverterInterface. Verification is intentionally omitted
        // to avoid the extra deserialization overhead.

        auto fb_data =
            flatbuffers::GetRoot<typename Converter::FBRootType>(buffer.get())
                ->data();

        std::vector<typename Converter::ItemType> data;
        data.reserve(fb_data->size());

        for (auto elem : *fb_data) {
            data.emplace_back(this->converter_.convert_from_fb(elem));
        }

        return WrapperType(std::move(data));
    }
};
