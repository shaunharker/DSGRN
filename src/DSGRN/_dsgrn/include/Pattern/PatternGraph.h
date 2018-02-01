/// PatternGraph.h
/// MIT LICENSE
/// Shaun Harker and Bree Cummins
/// 2016-03-19

#pragma once

#include "common.h"

#include "Pattern/Pattern.h"

struct PatternGraph_;

class PatternGraph {
public:
  /// PatternGraph
  ///   Default constructor
  PatternGraph ( void );

  /// PatternGraph
  ///   Create a pattern graph from a pattern
  PatternGraph ( Pattern const& pattern );

  /// assign
  ///   Create a pattern graph from a poset
  void
  assign ( Pattern const& pattern );

  /// root
  ///   Return the index of the root vertex
  uint64_t 
  root ( void ) const;

  /// leaf
  ///   Return the index of the leaf vertex
  uint64_t 
  leaf ( void ) const;

  /// size
  ///   Return the number of vertices in the pattern graph
  uint64_t
  size ( void ) const;

  /// dimension
  uint64_t
  dimension ( void ) const;

  /// label
  ///   Given a vertex, return the associated label
  ///   The label is a 64 bit word with bits interpreted as follows:
  ///    bit i+D     bit i   interpretation
  ///         0        1    ith variable decreasing  
  ///         1        0    ith variable increasing
  ///         0        0    ith variable can either increase or decrease
  ///   Note the limitation to 32 dimensions. Here D is the number of
  ///   dimensions.
  uint64_t
  label ( uint64_t v ) const;
  
  /// consume
  ///   Given a vertex v and an edge label, report the unique vertex u, if it exists,
  ///   in the adjacency list of vertex with an identical (not just matching) edge label.
  ///   edge_label is only 1 on single bit; bit i means max (M) in i, bit i+D means min (m) in i
  uint64_t 
  consume ( uint64_t vertex, uint64_t edge_label ) const;

  /// graphviz
  ///   Return a graphviz representation of the pattern graph
  std::string
  graphviz ( void ) const;

  /// graphviz_with_highlighted_path
  ///   Return a graphviz representation of the graph with given path highlighted
  std::string 
  graphviz_with_highlighted_path ( std::vector<uint64_t> const& path ) const;

private:
  std::shared_ptr<PatternGraph_> data_;
};

struct PatternGraph_ {
  uint64_t root_;
  uint64_t leaf_;
  uint64_t size_;
  uint64_t dimension_;
  std::vector<uint64_t> labels_;
  std::vector<std::unordered_map<uint64_t, uint64_t>> consume_;
};

/// Python Bindings

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
namespace py = pybind11;

inline void
PatternGraphBinding (py::module &m) {
  py::class_<PatternGraph, std::shared_ptr<PatternGraph>>(m, "PatternGraph")
    .def(py::init<>())
    .def(py::init<Pattern const&>())
    .def("root", &PatternGraph::root)
    .def("leaf", &PatternGraph::leaf)
    .def("size", &PatternGraph::size)
    .def("dimension", &PatternGraph::dimension)
    .def("label", &PatternGraph::label)
    .def("consume", &PatternGraph::consume)
    .def("graphviz", &PatternGraph::graphviz)
    .def("graphviz_with_highlighted_path", &PatternGraph::graphviz_with_highlighted_path);
}
