/// Configuration.h
/// Shaun Harker
/// 2018-01-31

// Contains "Singleton" class with system-configuration information
// At this point, this consists of information about where the logic files are stored
// The python import module 

#pragma once

#include "common.h"

struct Configuration {
public:
  Configuration ( void ) {
    path_ = "/usr/local/share/DSGRN/logic";
  }
  void set_path ( std::string const& path ) {
    path_ = path;
  }
  std::string
  get_path ( void ) const {
    return path_;
  }

private:
  std::string path_;
};

std::shared_ptr<Configuration>
configuration ( void ) {
  static std::shared_ptr<Configuration> global_config ( new Configuration );
  return global_config;
}

/// Python Bindings

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
namespace py = pybind11;

inline void
ConfigurationBinding (py::module &m) {
  py::class_<Configuration, std::shared_ptr<Configuration>>(m, "Configuration")
    .def(py::init<>())
    .def("set_path", &Configuration::set_path)
    .def("get_path", &Configuration::get_path);
  m.def("configuration", &configuration);
}
