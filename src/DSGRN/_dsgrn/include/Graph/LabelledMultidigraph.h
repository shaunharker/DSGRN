/// LabelledMultidigraph.h
/// Shaun Harker
/// 2018-02-22
/// MIT LICENSE

#pragma once

#include "common.h"

class LabelledMultidigraph {
public:

  typedef char LabelType;

  /// LabelledMultidigraph
  LabelledMultidigraph ( void );

  /// add_vertex
  uint64_t
  add_vertex ( void );

  /// add_edge
  void
  add_edge ( uint64_t i, uint64_t j, LabelType l );

  /// num_vertices
  uint64_t
  num_vertices ( void ) const;

  /// adjacencies
  std::map<LabelType, std::unordered_set<uint64_t>> const&
  adjacencies( uint64_t v ) const;

  /// unlabelled_adjacencies
  std::unordered_set<uint64_t>
  unlabelled_adjacencies( uint64_t v ) const;

  /// graphviz
  std::string 
  graphviz ( void ) const;

private:
  std::vector<std::map<LabelType, std::unordered_set<uint64_t>>> adj_;
};

/// Python Bindings

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
namespace py = pybind11;

inline void
LabelledMultidigraphBinding (py::module &m) {
  py::class_<LabelledMultidigraph, std::shared_ptr<LabelledMultidigraph>>(m, "LabelledMultidigraph")
    .def(py::init<>())
    .def("add_vertex", &LabelledMultidigraph::add_vertex)
    .def("add_edge", &LabelledMultidigraph::add_edge)
    .def("num_vertices", &LabelledMultidigraph::num_vertices)
    .def("adjacencies", &LabelledMultidigraph::adjacencies)
    .def("unlabelled_adjacencies", &LabelledMultidigraph::unlabelled_adjacencies)
    .def("graphviz", &LabelledMultidigraph::graphviz);
}

