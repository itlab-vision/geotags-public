// Copyright 2025 itlab-vision
#pragma once

#include <vector>
#include <string>
#include <utility>

#include "reader_interface.hpp"  // NOLINT

template <typename Parser, typename ItemType, typename WrapperType>
class GenericReaderSTL : public ReaderInterface<WrapperType> {
 private:
    Parser parser_{};

 public:
    explicit GenericReaderSTL(const std::string& fname)
        : ReaderInterface<WrapperType>(fname) {}

    WrapperType read() override;
};

template <typename Parser, typename ItemType, typename WrapperType>
inline WrapperType GenericReaderSTL<Parser, ItemType, WrapperType>::read() {
    std::string items_size_str;
    std::getline(this->file, items_size_str);

    std::vector<ItemType> items;
    items.reserve(
        parser_.template convert_num_argument<size_t>(items_size_str));

    std::string header;
    std::getline(this->file, header);

    parser_.init_parser(header);

    while (!this->file.eof()) {
        this->line_buffer.clear();
        std::getline(this->file, this->line_buffer);

        ItemType item{};
        if (parser_.parse_line(item, this->line_buffer)) {
            items.push_back(std::move(item));
        }
    }
    return WrapperType(std::move(items));
}
