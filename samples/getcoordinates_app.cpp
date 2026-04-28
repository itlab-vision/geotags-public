// Copyright 2025 itlab-vision
#include <chrono>

#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>

#include "TinyEXIF.h"
#include "coordinates.hpp"       // NOLINT
#include "distance.hpp"          // NOLINT
#include "exif_wrapper.hpp"      // NOLINT
#include "locations_reader.hpp"  // NOLINT

int main(int argc, char* argv[]) {
    cv::CommandLineParser cmd(
        argc, argv,
        "{ i input            |           | Input image }"
        "{ h help             |           | Print help message }");

    std::string image_path = cmd.get<std::string>("input");
    if (image_path == "") {
        cmd.printMessage();
        return -3;
    }

    try {
        typedef std::chrono::steady_clock timer;
        auto tstart = timer::now();
        EXIFData exif_data(image_path);
        auto tfinish = timer::now();
        std::chrono::duration<double> texif_parsing{tfinish - tstart};

        Coordinates location(exif_data.get_lat(), exif_data.get_lon());

        std::cout << "Latitude: " << location.latitude << std::endl;
        std::cout << "Longitude: " << location.longitude << std::endl;
        std::cout << "Time: " << texif_parsing.count() << " s" << std::endl;
    } catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
        return -1;
    }
    return 0;
}
