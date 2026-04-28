// Copyright 2025 itlab-vision
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "geotags_api.hpp"  // NOLINT

namespace py = pybind11;

PYBIND11_MODULE(geotags_module, m) {
    m.doc() = "C++ geotags app bindings";

    py::class_<Result>(m, "Result")
        .def_readonly("latitude", &Result::latitude)
        .def_readonly("longitude", &Result::longitude)
        .def_readonly("nearest_location", &Result::nearest_location)
        .def_readonly("distance", &Result::distance);

    m.def("get_location", &get_location, py::arg("image_path"),
          py::arg("reader_type"), py::arg("loc_file"), py::arg("dist_formula"),
          py::arg("search_type"));
}
