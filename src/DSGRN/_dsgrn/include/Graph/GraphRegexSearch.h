/// GraphRegexSearch.h
/// Shaun Harker
/// 2018-02-20
/// MIT LICENSE

namespace NFA_detail {
  typedef char LabelType;
  class NFA_Node;
}

class NFA {
public:
  typedef NFA_detail::LabelType LabelType;

  /// NFA
  NFA ( void );

  /// NFA
  NFA ( std::string const& regex );

  /// graphviz
  std::string 
  graphviz ( void ) const;

  /// add_vertex
  uint64_t
  add_vertex ( void );

  /// add_edge
  void
  add_edge ( uint64_t i, uint64_t j, LabelType l );

  /// num_vertices
  uint64_t
  num_vertices ( void ) const;

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
  std::vector<std::unordered_map<LabelType, std::unordered_set<uint64_t>>> adj_;
  uint64_t initial_idx_;
  uint64_t final_idx_;
};

/// Python Bindings

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
namespace py = pybind11;

inline void
GraphRegexSearchBinding (py::module &m) {
  py::class_<NFA, std::shared_ptr<NFA>>(m, "NFA")
    .def(py::init<>())
    .def(py::init<std::string const&>())
    .def("graphviz", &NFA::graphviz);
}
