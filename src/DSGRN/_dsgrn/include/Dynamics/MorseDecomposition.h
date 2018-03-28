/// MorseDecomposition.h
/// Shaun Harker
/// 2015-05-24

#pragma once

#ifndef INLINE_IF_HEADER_ONLY
#define INLINE_IF_HEADER_ONLY
#endif

#include "common.h"

#include "Graph/Digraph.h"
#include "Graph/Components.h"
#include "Graph/Poset.h"

struct MorseDecomposition_;

class MorseDecomposition : public TypedObject {
public:

  /// type
  virtual std::string type (void) const final { return "MorseDecomposition"; }

  /// constructor
  MorseDecomposition ( void );

  /// MorseDecomposition
  ///   Create a Morse Decomposition given
  ///   a digraph.
  MorseDecomposition ( Digraph const& digraph );

  /// MorseDecomposition
  ///   Create a Morse Decomposition given
  ///   a digraph and its strong components
  ///   (This method is provided in case 
  ///    strong components are already computed.)
  MorseDecomposition ( Digraph const& digraph, 
                       Components const& components );

  /// assign
  ///   Create a Morse Decomposition given
  ///   a digraph.
  void
  assign ( Digraph const& digraph );

  /// assign
  ///   Create a Morse Decomposition given
  ///   a digraph and its strong components
  ///   (This method is provided in case 
  ///    strong components already computed.)
  void
  assign ( Digraph const& digraph, 
           Components const& components );

  /// poset 
  ///   Return the underlying partial order
  Poset const
  poset ( void ) const;

  /// components 
  ///   Return all components 
  Components const
  components ( void ) const;

  /// components 
  ///   Return recurrent components 
  Components::ComponentContainer const
  recurrent ( void ) const;

  /// morseset
  ///   Return a Morse set
  std::vector<uint64_t>
  morseset ( uint64_t i ) const;
  
  /// graphviz
  ///   Return a graphviz representation
  std::string
  graphviz ( void ) const;

  /// operator <<
  ///   Stream information to graphviz format
  friend std::ostream& operator << ( std::ostream& stream, MorseDecomposition const& md );

private:
  std::shared_ptr<MorseDecomposition_> data_;
  /// _canonicalize
  ///   Renumber the vertices in a "canonical" fashion
  ///   given the available information. (Note: not every
  ///   isomorphic graph has the same canonicalization.)
  void _canonicalize ( void );
};

struct MorseDecomposition_ {
  Components components_;
  Poset poset_;
};

/// Python Bindings

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
namespace py = pybind11;

inline void
MorseDecompositionBinding (py::module &m) {
  py::class_<MorseDecomposition, std::shared_ptr<MorseDecomposition>, TypedObject>(m, "MorseDecomposition")
    .def(py::init<>())
    .def(py::init<Digraph const&>())
    .def(py::init<Digraph const&,Components const&>())    
    //.def("assign", (void(MorseDecomposition::*)(std::shared_ptr<Digraph>))&Complex::assign)
    //.def("assign", (void(MorseDecomposition::*)(std::shared_ptr<Digraph>,std::shared_ptr<Components>))&Complex::assign)    
    .def("poset", &MorseDecomposition::poset)
    .def("components", &MorseDecomposition::components)
    .def("recurrent", &MorseDecomposition::recurrent)
    .def("morseset", &MorseDecomposition::morseset)
    .def("graphviz", &MorseDecomposition::graphviz);
}
