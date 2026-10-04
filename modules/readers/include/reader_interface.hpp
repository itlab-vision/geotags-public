// Copyright 2025 itlab-vision
#pragma once

#include <string>
#include <stdexcept>
#include <fstream>

template <typename WrapperType>
class ReaderInterface {
 public:
    virtual WrapperType read() = 0;

 protected:
    std::string line_buffer;
    std::string file_name;
    std::ifstream file;

    explicit ReaderInterface(const std::string& fname) : file_name(fname) {
        line_buffer.reserve(4096);
        this->open_file();
    }

    void open_file() {
        file = std::ifstream(file_name);
        if (!file) {
            throw std::invalid_argument("Can't open file: " + file_name);
        }
    }
};
