// Copyright 2025 itlab-vision
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "geotags_api.hpp"  // NOLINT

namespace py = pybind11;

PYBIND11_MODULE(geotags_module, m) {
    m.doc() = "C++ geotags app bindings";

    py::class_<LocationInfo>(m, "LocationInfo")
        .def_readonly("location_id", &LocationInfo::location_id)
        .def_readonly("country", &LocationInfo::country)
        .def_readonly("city", &LocationInfo::city)
        .def_readonly("region_id", &LocationInfo::region_id)
        .def_readonly("district_id", &LocationInfo::district_id)
        .def_readonly("alt_names", &LocationInfo::alt_names)
        .def_readonly("attraction_db_path", &LocationInfo::attraction_db_path);

    py::class_<ResultLocation>(m, "Result")
        .def_readonly("latitude", &ResultLocation::latitude)
        .def_readonly("longitude", &ResultLocation::longitude)
        .def_readonly("nearest_location", &ResultLocation::nearest_location)
        .def_readonly("distance", &ResultLocation::distance);

    m.def("get_location", &get_location, py::arg("image_path"),
          py::arg("reader_type"), py::arg("loc_file"), py::arg("dist_formula"),
          py::arg("search_type"));

    py::class_<AttractionInfo>(m, "AttractionInfo")
        .def_readonly("type", &AttractionInfo::type)
        .def_readonly("name", &AttractionInfo::name)
        .def_readonly("extra_fields", &AttractionInfo::extra_fields);

    py::class_<ResultAttraction>(m, "AttractionResult")
        .def_readonly("latitude", &ResultAttraction::latitude)
        .def_readonly("longitude", &ResultAttraction::longitude)
        .def_readonly("nearest_attraction",
                      &ResultAttraction::nearest_attraction)
        .def_readonly("distance", &ResultAttraction::distance);

    m.def("get_attraction", &get_attraction, py::arg("image_path"),
          py::arg("reader_type"), py::arg("loc_file"), py::arg("dist_formula"),
          py::arg("search_type"));
}
