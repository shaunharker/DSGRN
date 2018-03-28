/// common.h
/// Shaun Harker
/// 2015-05-31

#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>
#include <vector>
#include <string>
#include <cstdint>
#include <cstdlib>
#include <algorithm>
#include <stack>
#include <functional>
#include <utility>
#include <cctype>
#include <exception>
#include <stdexcept>
#include <set>
#include <unordered_set>
#include <unordered_map>
#include <regex>

class TypedObject {
public:
  virtual std::string type (void) const { return "TypedObject"; }
};

/// Python Bindings

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
namespace py = pybind11;
inline void
TypedObjectBinding (py::module &m) {
  py::class_<TypedObject, std::shared_ptr<TypedObject>>(m, "TypedObject")
    .def("type", &TypedObject::type);
}

// #include "Tools/sqlambda.h"
#include "Tools/SHA256.h"
#include "Tools/json.hpp"
#include "Tools/hash.hpp"



using json = nlohmann::json;
