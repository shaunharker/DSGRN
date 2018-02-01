/// SearchGraph.h
/// MIT LICENSE
/// Shaun Harker and Bree Cummins
/// 2016-03-19

#pragma once

#include "common.h"

#include "Phase/DomainGraph.h"
#include "Graph/Digraph.h"

struct SearchGraph_;

class SearchGraph {
public:
  /// SearchGraph
  ///   Default constructor
  SearchGraph ( void );

  /// SearchGraph
  ///   Create search graph from a domain graph and a morse set index
  SearchGraph ( DomainGraph dg );

  /// SearchGraph
  ///   Create search graph from a domain graph and a morse set index
  SearchGraph ( DomainGraph dg, uint64_t morse_set_index );

  /// SearchGraph
  ///   Create search graph from a sequence of labels
  ///   (This is used for checking for a pattern in a time series)
  SearchGraph ( std::vector<uint64_t> const& labels, uint64_t dim );

  /// assign
  ///   Create search graph from a domain graph
  void
  assign ( DomainGraph dg );

  /// assign
  ///   Create search graph from a domain graph and a morse set index
  void
  assign ( DomainGraph dg, uint64_t morse_set_index );

  /// assign
  ///   Create search graph from a sequence of labels
  ///   (This is used for checking for a pattern in a time series)
  void
  assign ( std::vector<uint64_t> const& labels, uint64_t dim );

  /// size
  ///   Return the number of vertices in the search graph
  uint64_t
  size ( void ) const;

  /// dimension
  ///   Return the number of variables
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

  /// adjacencies
  ///   Given a vertex v, return vector of vertices which 
  ///   are out-edge adjacencies of input v
  std::vector<uint64_t> const&
  adjacencies ( uint64_t v ) const;

  /// event
  ///   Given a source and target domain give the variable
  ///   associated with the extremal event which may occur
  ///   on that wall. If no event is possible, return -1
  uint64_t 
  event ( uint64_t source, uint64_t target ) const;

  /// graphviz
  ///   Return a graphviz representation of the search graph
  std::string
  graphviz ( void ) const;

  /// graphviz_with_highlighted_path
  ///   Return a graphviz representation of the graph with given path highlighted
  std::string 
  graphviz_with_highlighted_path ( std::vector<uint64_t> const& path ) const;

  /// vertexInformation
  ///   Return information about a search graph vertex
  ///   This method is intended for debug purposes
  std::string
  vertexInformation ( uint64_t v ) const;

  /// edgeInformation
  ///   Return information about a search graph edge
  ///   This method is intended for debug purposes
  std::string
  edgeInformation ( uint64_t source, uint64_t target ) const;

private:
  std::shared_ptr<SearchGraph_> data_;
};

struct SearchGraph_ {
  Digraph digraph_;
  std::vector<uint64_t> labels_;
  std::vector<std::unordered_map<uint64_t, uint64_t>> event_;
  uint64_t dimension_;
  std::function<std::string(uint64_t)> vertex_information_;
  std::function<std::string(uint64_t,uint64_t)> edge_information_;
};

/// Python Bindings

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
namespace py = pybind11;

inline void
SearchGraphBinding (py::module &m) {
  py::class_<SearchGraph, std::shared_ptr<SearchGraph>>(m, "SearchGraph")
    .def(py::init<>())
    .def(py::init<DomainGraph const&>())
    .def(py::init<DomainGraph const&, uint64_t>())
    .def(py::init<std::vector<uint64_t> const&, uint64_t>())
    .def("size", &SearchGraph::size)
    .def("dimension", &SearchGraph::dimension)
    .def("label", &SearchGraph::label)
    .def("adjacencies", &SearchGraph::adjacencies)
    .def("event", &SearchGraph::event)
    .def("graphviz", &SearchGraph::graphviz)
    .def("graphviz_with_highlighted_path", &SearchGraph::graphviz_with_highlighted_path)
    .def("vertexInformation", &SearchGraph::vertexInformation)
    .def("edgeInformation", &SearchGraph::edgeInformation);
}
