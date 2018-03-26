/// NFA.h
/// Shaun Harker
/// 2018-02-20
/// MIT LICENSE

#include "LabelledMultidigraph.h"

namespace NFA_detail {
  typedef char LabelType;
  class NFA_Node;
}

class NFA : public LabelledMultidigraph {
public:
  typedef NFA_detail::LabelType LabelType;

  /// NFA
  NFA ( std::string const& regex );

  /// graphviz (override)
  std::string 
  graphviz ( void ) const;

private:
  /// NFA
  NFA ( void );

  /// NFA
  NFA (char a);
  
  /// NFA
  NFA ( NFA_detail::NFA_Node * initial, NFA_detail::NFA_Node * final );

  /// finalize
  void 
  finalize ( void );
  
  /// initial
  NFA_detail::NFA_Node *
  initial ( void ) const;

  /// final
  NFA_detail::NFA_Node *
  final ( void ) const;

  /// nodes
  std::unordered_set<NFA_detail::NFA_Node *>
  nodes ( void );

  /// delete_nodes
  void
  delete_nodes ( void );

  /// contract
  ///   Contract nodes together to give an equivalent but simpler NFA
  void
  contract ( void );

  /// alternation
  static NFA
  alternation ( NFA && lhs, NFA && rhs);

  /// concatenation
  static NFA
  concatenation ( NFA && lhs, NFA && rhs);

  /// kleene
  static NFA
  kleene ( NFA && nfa );

  /// plus
  static NFA
  plus ( NFA && nfa );

  /// question
  static NFA
  question ( NFA && nfa );

  // data members used during construction
  NFA_detail::NFA_Node * initial_;
  NFA_detail::NFA_Node * final_;
  // finalized data
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
    .def(py::init<std::string const&>())
    .def("graphviz", &NFA::graphviz);
}
