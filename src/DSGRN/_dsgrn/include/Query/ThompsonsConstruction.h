/// ThompsonsConstruction.h
/// Shaun Harker
/// 2018-02-20
/// MIT LICENSE

#pragma once

#include "common.h"
#include "Graph/LabelledMultidigraph.h"
#include "Query/NFA.h"

/// CompileRegexToNFA
NFA
CompileRegexToNFA (std::string const& regex);

/// Python Bindings

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
namespace py = pybind11;

inline void
ThompsonsConstructionBinding (py::module &m) {
  m.def("CompileRegexToNFA", &CompileRegexToNFA);
}

