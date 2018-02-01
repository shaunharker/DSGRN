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
  /// Used for TestPoset only
  MorseGraph ( Poset const & ps,
               std::unordered_map<uint64_t, Annotation> const & annotations );

  /// assign (Morse Decomposition)
  MorseGraph ( DomainGraph const& sg,
               MorseDecomposition const& md );

  /// assign (Morse Decomposition)
  MorseGraph ( WallGraph const& sg,
               MorseDecomposition const& md );

  /// assign (Morse Decomposition)
  ///   Assign data to Morse Graph
  template < class SearchGraph >
  void
  assign ( SearchGraph const& sg,
           MorseDecomposition const& md );

  /// poset
  ///   Access poset
  Poset const
  poset ( void ) const;

  /// annotation
  ///   Return the annotation on vertex v.
  Annotation const
  annotation ( uint64_t v ) const;

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
  void
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

// template < class SwitchingGraph > MorseGraph::
// MorseGraph ( SwitchingGraph const& sg,
//              MorseDecomposition const& md ) {
//   assign ( sg, md );
// }

template <class SearchGraph>
void MorseGraph::
assign ( SearchGraph const& sg,
         MorseDecomposition const& md ) {
  data_ . reset ( new MorseGraph_ );
  // Copy the poset
  data_ -> poset_ = md . poset ();
  // Compute the annotations
  uint64_t N = data_ -> poset_ . size ();
  for ( uint64_t v = 0; v < N; ++ v ) {
    data_ -> annotations_[v] = sg . annotate ( md . recurrent () [ v ] );
  }
  // Canonicalize the graph
  _canonicalize ();
}

/// Python Bindings

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
namespace py = pybind11;

inline void
MorseGraphBinding (py::module &m) {
  py::class_<MorseGraph, std::shared_ptr<MorseGraph>>(m, "MorseGraph")
    .def(py::init<>())
    .def(py::init<Poset const&,std::unordered_map<uint64_t,Annotation>>())
    .def(py::init<DomainGraph const&,MorseDecomposition const&>())
    .def(py::init<WallGraph const&,MorseDecomposition const&>())        
    .def("poset", &MorseGraph::poset)
    .def("annotation", &MorseGraph::annotation)
    .def("SHA256", &MorseGraph::SHA256)
    .def("stringify", &MorseGraph::stringify)
    .def("parse", &MorseGraph::parse)
    .def("graphviz", &MorseGraph::graphviz);
}
