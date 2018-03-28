/// MorseGraph.h
/// Shaun Harker
/// 2015-05-24

#pragma once

#include "common.h"

#include "Dynamics/MorseDecomposition.h"
#include "Dynamics/Annotation.h"
#include "Graph/Poset.h"
#include "Phase/DomainGraph.h"
#include "Phase/WallGraph.h"

struct MorseGraph_;

/// class MorseGraph
class MorseGraph  {
public:
  /// constructor
  MorseGraph ( void );

  /// constructor from a poset and annotations
  MorseGraph ( Poset const& ps,
               std::unordered_map<uint64_t, Annotation> const& annotations );

  /// MorseGraph (Morse Decomposition)
  ///   Arguments should be a MorseDecomposition and associated DomainGraph (or WallGraph)
  ///   given in either order
  MorseGraph ( TypedObject const& a,
               TypedObject const& b );

  /// MorseGraph (Morse Decomposition)
  ///   Arguments should be either a DomainGraph (or a WallGraph)
  ///   Note: A MorseDecomposition object will be computed and discarded
  MorseGraph ( TypedObject const& sg );

  /// assign (Morse Decomposition)
  ///   Assign data to Morse Graph
  ///   Arguments should be a MorseDecomposition and the associated DomainGraph (or WallGraph)
  ///   given in either order
  void
  assign ( TypedObject const& sg,
           TypedObject const& md );

  /// poset
  ///   Access poset
  Poset const
  poset ( void ) const;

  /// annotation
  ///   Return the annotation on vertex v.
  Annotation const
  annotation ( uint64_t v ) const;

  /// annotations
  ///   Access annotations data
  std::unordered_map<uint64_t, Annotation> const&
  annotations ( void ) const;

  /// SHA
  ///   Return a SHA-256 code
  std::string
  SHA256 ( void ) const;

  /// stringify
  ///   Return a JSON description
  std::string
  stringify ( void ) const;

  /// parse
  ///   Initialize from a JSON description
  MorseGraph &
  parse ( std::string const& str );

  /// graphviz
  ///   Return a graphviz representation of the Morse graph
  std::string
  graphviz ( void ) const;

  /// operator <<
  ///   Stream information to graphviz format
  friend std::ostream& operator << ( std::ostream& stream, MorseGraph const& md );

private:
  std::shared_ptr<MorseGraph_> data_;
  /// _canonicalize
  ///   Renumber the vertices in a "canonical" fashion
  ///   given the available information. (Note: not every
  ///   isomorphic graph has the same canonicalization.)
  void _canonicalize ( void );
};

struct MorseGraph_ {
  Poset poset_;
  std::unordered_map<uint64_t, Annotation> annotations_;
};

/// Python Bindings

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
namespace py = pybind11;

inline void
MorseGraphBinding (py::module &m) {
  py::class_<MorseGraph, std::shared_ptr<MorseGraph>>(m, "MorseGraph")
    .def(py::init<>())
    .def(py::init<Poset const&,std::unordered_map<uint64_t,Annotation>>())
    .def(py::init<TypedObject const&,TypedObject const&>())
    .def(py::init<TypedObject const&>())
    .def("assign", &MorseGraph::assign)
    .def("poset", &MorseGraph::poset)
    .def("annotation", &MorseGraph::annotation)
    .def("SHA256", &MorseGraph::SHA256)
    .def("__str__", &MorseGraph::stringify)
    .def("stringify", &MorseGraph::stringify)
    .def("parse", &MorseGraph::parse)
    .def("graphviz", &MorseGraph::graphviz)
    .def(py::pickle(
    [](MorseGraph const& p) { // __getstate__
        /* Return a tuple that fully encodes the state of the object */
        return py::make_tuple(p.poset(), p.annotations());
    },
    [](py::tuple t) { // __setstate__
        if (t.size() != 2)
            throw std::runtime_error("Unpickling Parameter object: Invalid state!");
        /* Create a new C++ instance */
        return MorseGraph(t[0].cast<Poset>(), t[1].cast<std::unordered_map<uint64_t, Annotation>>());
    }));
}
