/// StrongComponents
/// Shaun Harker
/// 2015-05-24

#pragma once

#include "common.h"

#include "Graph/Components.h"
#include "Graph/Digraph.h"

Components
StrongComponents ( Digraph const digraph );

/// Python Bindings

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
namespace py = pybind11;

inline
void StrongComponentsBinding(py::module &m) {
  m.def("StrongComponents", &StrongComponents);
}
