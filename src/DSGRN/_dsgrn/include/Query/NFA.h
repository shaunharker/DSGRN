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
  NFA ( void );

  /// set initial
  void
  set_initial ( Integer idx ) { initial_idx_ = idx; }
  
  /// set_final
  void
  set_final ( Integer idx ) { final_idx_ = idx; }
  
  ///
  Integer
  initial ( void ) { return initial_idx_; }

  Integer
  final ( void ) { return final_idx_; }
  
  /// graphviz (override)
  std::string 
  graphviz ( void ) const;

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
    .def(py::init<void>())
    .def("set_initial", &NFA::set_initial)
    .def("set_final", &NFA::set_final)
    .def("initial", &NFA::initial)
    .def("final", &NFA::final)
    .def("graphviz", &NFA::graphviz);
}
