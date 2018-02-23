/// TensorGraphProduct.h
/// Shaun Harker
/// 2018-02-23
/// MIT LICENSE

#pragma once 

#include "LabelledMultidigraph.h"

LabelledMultidigraph
TensorGraphProduct ( LabelledMultidigraph const& lhs, LabelledMultidigraph const& rhs );

/// Python Bindings

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
namespace py = pybind11;

inline
void TensorGraphProductBinding(py::module &m) {
  m.def("TensorGraphProduct", &TensorGraphProduct);
}
