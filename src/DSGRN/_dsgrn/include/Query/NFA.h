/// NFA.h
/// Shaun Harker
/// 2018-05-17
/// MIT LICENSE

#pragma once

#include "common.h"
#include "Graph/LabelledMultidigraph.h"

namespace NFA_detail {
  typedef char LabelType;
  class NFA_Node;
}

/// NFA
///   nondeterministic finite automata with epsilon transitions
///   note: epsilon transitions have ' ' character
class NFA : public LabelledMultidigraph {
public:
  typedef NFA_detail::LabelType LabelType;

  /// NFA
  NFA ( void );

  /// set initial
  void
  set_initial ( uint64_t idx );
  
  /// set_final
  void
  set_final ( uint64_t idx );
  
  /// initial
  ///   Returns start node of automata
  uint64_t
  initial ( void ) const;

  /// final
  ///   Returns accept node of automata
  uint64_t
  final ( void ) const;
  
  /// intersect
  ///   Return {nfa, \pi }, where pi[v] is the pair of vertices used in the product construction
  static std::pair<NFA, std::vector<std::pair<uint64_t,uint64_t>>>
  intersect ( NFA const& lhs, NFA const& rhs );

  /// graphviz (override)
  std::string 
  graphviz ( void ) const;

  /// count_paths
  ///    Return number of accepting paths (if finite)
  uint64_t
  count_paths ( void ) const;


private:
  uint64_t initial_idx_;
  uint64_t final_idx_;
};

/// Python Bindings

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
namespace py = pybind11;

inline void
NFABinding (py::module &m) {
  py::class_<NFA, std::shared_ptr<NFA>>(m, "NFA")
    .def(py::init<>())
    .def("add_vertex", &NFA::add_vertex)
    .def("add_edge", &NFA::add_edge)
    .def("num_vertices", &NFA::num_vertices)
    .def("adjacencies", &NFA::adjacencies)
    .def("unlabelled_adjacencies", &NFA::unlabelled_adjacencies)
    .def("set_initial", &NFA::set_initial)
    .def("set_final", &NFA::set_final)
    .def("initial", &NFA::initial)
    .def("final", &NFA::final)
    .def_static("intersect", &NFA::intersect)
    .def("graphviz", &NFA::graphviz);
}
