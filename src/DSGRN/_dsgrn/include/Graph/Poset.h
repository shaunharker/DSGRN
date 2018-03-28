/// Poset.h
/// Shaun Harker
/// 2015-05-15

#pragma once

#include "common.h"

#include "Graph/Digraph.h"

struct Poset_;

/// class Poset
class Poset {
public:
  /// Poset
  ///   Default constructor
  Poset ( void );

  /// Poset
  ///   Construct from adjacency lists
  Poset ( std::vector<std::vector<uint64_t>> & adjacencies );

  /// Poset
  ///   Construct from digraph
  Poset ( Digraph const& digraph );

  /// assign
  ///   Construct from adjacency lists
  void
  assign ( std::vector<std::vector<uint64_t>> & adjacencies );

  /// assign
  ///   Construct from digraph
  void
  assign ( Digraph const& digraph );

  /// size
  ///   Return number of vertices
  uint64_t
  size ( void ) const;

  /// parents
  ///   Return vector of vertices which are the parents of v
  ///   in the Hasse diagram of the poset
  std::vector<uint64_t> const&
  parents ( uint64_t v ) const;

  /// children
  ///   Return vector of vertices which are the children of v
  ///   in the Hasse diagram of the poset
  std::vector<uint64_t> const&
  children ( uint64_t v ) const;

  /// ancestors
  ///   Return vector of vertices which are the ancestors of v
  std::vector<uint64_t> const&
  ancestors ( uint64_t v ) const;

  /// descendants
  ///   Return vector of vertices which are the descendants of v
  std::vector<uint64_t> const&
  descendants ( uint64_t v ) const;

  /// maximal
  ///   Given a collection of elements, return the subset of those elements
  ///   which are maximal in the set
  std::set<uint64_t>
  maximal ( std::set<uint64_t> const& elements ) const;

  /// compare
  ///   Check if vertex u < vertex v in poset
  bool
  compare ( const uint64_t & u, const uint64_t & v ) const;

  /// permute
  ///   Reorder the digraph according to the provided permutation
  ///   The convention on the permutation is that vertex v in the input 
  ///   corresponds to vertex permutation[v] in the output
  Poset
  permute ( const std::vector<uint64_t> & permutation ) const;

  /// stringify
  ///   Return a JSON description
  std::string
  stringify ( void ) const;

  /// parse
  ///   Initialize from a JSON description
  ///   Format: A JSON description of a Digraph object
  ///           The digraph is one suitable for use in
  ///           the constructor method.
  Poset &
  parse ( std::string const& str );

  /// graphviz
  ///   Return a graphviz representation of the poset (as Hasse diagram)
  std::string
  graphviz ( void ) const;
  
  /// operator <<
  ///   Emit data to stream in graphviz format
  friend std::ostream& operator << ( std::ostream& stream, Poset const& poset );

private:
  std::shared_ptr<Poset_> data_;
};

struct Poset_ {
  Digraph transitive_closure;
  Digraph transitive_reduction;
  Digraph transpose_transitive_closure;
  Digraph transpose_transitive_reduction;
  std::vector<std::unordered_set<uint64_t>> relation;
};

/// Python Bindings

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
namespace py = pybind11;

inline void
PosetBinding (py::module &m) {
  py::class_<Poset, std::shared_ptr<Poset>>(m, "Poset")
    .def(py::init<>())
    .def(py::init<std::vector<std::vector<uint64_t>> &>())
    .def(py::init<Digraph const&>())
    .def("size", &Poset::size)
    .def("parents", &Poset::parents)
    .def("children", &Poset::children)
    .def("ancestors", &Poset::ancestors)
    .def("descendants", &Poset::descendants)
    .def("maximal", &Poset::maximal)
    .def("compare", &Poset::compare)
    .def("permute", &Poset::permute)
    .def("stringify", &Poset::stringify)
    .def("parse", &Poset::parse)
    .def("graphviz", &Poset::graphviz)
    .def(py::pickle(
    [](Poset const& p) { // __getstate__
        /* Return a tuple that fully encodes the state of the object */
        return py::make_tuple(p.stringify());
    },
    [](py::tuple t) { // __setstate__
        if (t.size() != 1)
            throw std::runtime_error("Unpickling Parameter object: Invalid state!");
        /* Create a new C++ instance */
        return Poset().parse(t[0].cast<std::string>());
    }));
}
