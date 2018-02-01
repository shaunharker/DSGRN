/// MatchingGraph.h
/// MIT LICENSE
/// Shaun Harker and Bree Cummins
/// 2016-03-19

#pragma once

#include "common.h"

#include "Pattern/PatternGraph.h"
#include "Pattern/SearchGraph.h"

struct MatchingGraph_;

class MatchingGraph {
public:
  /// MatchingGraph
  ///   Default constructor
  MatchingGraph ( void );

  /// MatchingGraph
  ///   Create a matching graph from a search graph and a pattern graph
  MatchingGraph ( SearchGraph const& sg, PatternGraph const& pg );

  /// assign
  ///   Create a matching graph from a search graph and a pattern graph
  void
  assign ( SearchGraph const& sg, PatternGraph const& pg );

  /// SearchGraph
  ///   Return associated search graph
  SearchGraph const&
  searchgraph ( void ) const;

  /// PatternGraph
  ///   Return associated pattern graph
  PatternGraph const&
  patterngraph ( void ) const;

  /// Vertex
  ///   vertex data type. The first entry correspond to a vertex
  ///   in the search graph (which we call a domain), and the
  ///   second entry is the pattern graph index, which we call
  ///   position 
  typedef std::pair<uint64_t,uint64_t> Vertex;

  /// query
  ///   Given a (domain, position) pair, determine if it is
  ///   a vertex in the matching graph
  bool
  query ( Vertex const& v ) const;
  
  /// adjacencies
  ///   Given a vertex v, return vector of vertices which 
  ///   are out-edge adjacencies of input v
  std::vector<Vertex>
  adjacencies ( Vertex const& v ) const;

  /// roots
  ///   Give the elements of the form (domain, root)
  ///   in the matching graph
  std::vector<Vertex>
  roots ( void ) const;
  
  /// domain
  ///   Given a vertex v, return the associated domain (i.e. vertex in search graph)
  ///   Note this domain index may not be consistent with the indexing in the domain graph
  ///   since we have taken a subgraph
  uint64_t
  domain ( Vertex const& v ) const;

  /// position
  ///   Given a vertex v, return the associated position (i.e. vertex in the pattern graph)
  uint64_t 
  position ( Vertex const& v ) const;

  /// vertex
  ///   Given a domain and position, return the associated vertex in the matching graph
  ///   Note: will 
  Vertex
  vertex ( uint64_t domain, uint64_t position ) const;

  /// graphviz
  ///   Return a graphviz representation of the matching graph
  std::string
  graphviz ( void ) const;

  /// graphviz_with_highlighted_path
  ///   Return a graphviz representation of the matching graph with given path highlighted
  std::string 
  graphviz_with_highlighted_path ( std::vector<MatchingGraph::Vertex> const& path ) const;

private:
  std::shared_ptr<MatchingGraph_> data_;
};

struct MatchingGraph_ {
  PatternGraph pg_;
  SearchGraph sg_;
};

/// Python Bindings

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
namespace py = pybind11;

inline void
MatchingGraphBinding (py::module &m) {
  py::class_<MatchingGraph, std::shared_ptr<MatchingGraph>>(m, "MatchingGraph")
    .def(py::init<>())
    .def(py::init<SearchGraph const&,PatternGraph const&>())
    .def("searchgraph", &MatchingGraph::searchgraph)
    .def("patterngraph", &MatchingGraph::patterngraph)
    .def("query", &MatchingGraph::query)
    .def("adjacencies", &MatchingGraph::adjacencies)
    .def("roots", &MatchingGraph::roots)
    .def("domain", &MatchingGraph::domain)
    .def("position", &MatchingGraph::position)
    .def("vertex", &MatchingGraph::vertex)
    .def("graphviz", &MatchingGraph::graphviz)
    .def("graphviz_with_highlighted_path", &MatchingGraph::graphviz_with_highlighted_path);
}
