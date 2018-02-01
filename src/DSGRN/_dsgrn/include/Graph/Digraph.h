/// Digraph.h
/// Shaun Harker
/// 2015-05-24

#pragma once

#include "common.h"

struct Digraph_;

/// class Digraph
///   This class handles storage of edges between
///   vertices in the form of adjacency lists.
///  NOTE: the adjacency lists will be sorted if the Digraph(adjacencies) or parse
///        constructor is used. However if the default constructor is used followed by
///        add_vertex and add_edge operations there is no guarantee that the adjacency
///        lists are sorted. 
class Digraph {
public:
  /// Digraph
  ///   default constructor
  Digraph ( void );

  /// Digraph
  ///   construct a Digraph from an adjacency list
  Digraph ( std::vector<std::vector<uint64_t>> const& adjacencies );

  /// assign
  ///   construct a Digraph from an adjacency list
  void
  assign ( std::vector<std::vector<uint64_t>> const& adjacencies );

  /// adjacencies
  ///   Return vector of Vertices which are out-edge adjacencies of input v
  std::vector<uint64_t> const&
  adjacencies ( uint64_t v ) const;

  /// size
  ///   Return number of vertices
  uint64_t
  size ( void ) const;

  /// resize(n)
  ///   Resize so there n vertices.
  ///   Creates empty adjacency lists if needed.
  ///   n must not be smaller than current size
  void
  resize ( uint64_t n );

  /// add_vertex
  ///   Add a vertex, and return the
  ///   index of the newly added vertex.
  uint64_t
  add_vertex ( void );

  /// add_edge
  ///   Add an edge between source and target
  ///   Warning: does not check if edge already exists
  void
  add_edge ( uint64_t source, uint64_t target );

  /// finalize
  ///   sort the adjacency lists in ascending order
  void
  finalize ( void );

  /// transpose
  ///   Return the transpose graph (https://en.wikipedia.org/wiki/Transpose_graph)
  Digraph
  transpose ( void ) const;

  /// transitive_reduction
  ///   Return the transitive reduction
  ///   Note: requires the graph be a transitively closed DAG
  ///         or else a transitively closed DAG with extra self-edges
  Digraph
  transitive_reduction ( void ) const;

  /// transitive_closure
  ///   Compute the transitive closure.
  ///   Note: self-edges are not in the result even if they are in the input
  Digraph
  transitive_closure ( void ) const;

  /// permute
  ///   Reorder the digraph according to the provided permutation
  ///   The convention on the permutation is that vertex v in the input 
  ///   corresponds to vertex permutation[v] in the output
  Digraph
  permute ( const std::vector<uint64_t> & permutation ) const;

  /// stringify
  ///   Return a JSON description
  std::string
  stringify ( void ) const;

  /// parse
  ///   Initialize from a JSON description
  ///   Format: an array of arrays of integers.
  ///           The inner arrays are adjacency lists.
  ///           Vertices are numbered 0...N-1
  ///           The outer array has length N
  void
  parse ( std::string const& str );

  /// graphviz
  ///   Return a graphviz representation of the digraph
  std::string
  graphviz ( void ) const;

  /// operator <<
  ///   Emit data to stream in graphviz format
  friend std::ostream& operator << ( std::ostream& stream, Digraph const& dg );

protected:
  std::shared_ptr<Digraph_> data_;
};

struct Digraph_ {
  std::vector<std::vector<uint64_t>> adjacencies_;
};


/// Python Bindings

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
namespace py = pybind11;

inline void
DigraphBinding (py::module &m) {
  py::class_<Digraph, std::shared_ptr<Digraph>>(m, "Digraph")
    .def(py::init<>())
    .def(py::init<std::vector<std::vector<uint64_t>> const&>())
    .def("adjacencies", &Digraph::adjacencies)    
    .def("size", &Digraph::size)
    .def("resize", &Digraph::resize)
    .def("add_vertex", &Digraph::add_vertex)      
    .def("add_edge", &Digraph::add_edge)
    .def("finalize", &Digraph::finalize)
    .def("transpose", &Digraph::transpose)
    .def("transitive_reduction", &Digraph::transitive_reduction)
    .def("transitive_closure", &Digraph::transitive_closure)
    .def("permute", &Digraph::permute)
    .def("stringify", &Digraph::stringify)
    .def("parse", &Digraph::parse)
    .def("graphviz", &Digraph::graphviz);
}
