/// Pattern.h
/// MIT LICENSE
/// Shaun Harker and Bree Cummins
/// 2016-03-21

#pragma once

#include "common.h"

#include "Graph/Poset.h"

struct Pattern_;

class Pattern {
public:
  /// Pattern
  ///   Default constructor
  Pattern ( void );

  /// Pattern
  ///   Construct from a poset and event data
  ///   Inputs: "poset" is the poset of min/max events
  ///           "events" gives the map from poset elements to event
  ///                    variable indexing. (Note: it does not
  ///                    have min/max data.)
  ///           "final_label" gives the final state after
  ///                    all events. Further details below.
  ///           "dimension" is the number of variables
  ///   The label is a 64 bit word with bits interpreted as follows:
  ///    bit i+D     bit i   interpretation
  ///         0        1    ith variable decreasing  
  ///         1        0    ith variable increasing
  ///   Note the limitation to 32 dimensions. Here D is the number of
  ///   variables.
  Pattern ( Poset const& poset, 
            std::vector<uint64_t> const& events,
            uint64_t final_label,
            uint64_t dimension );

  /// assign
  ///   Construct from a poset and event data
  void
  assign ( Poset const& poset, 
           std::vector<uint64_t> const& events,
           uint64_t final_label, 
           uint64_t dimension );

  /// load
  ///   Load from a file containing a JSON string (see parse method)
  void
  load ( std::string const& filename );

  /// label
  ///   Return the label corresponding to the final state
  ///   The label is a 64 bit word with bits interpreted as follows:
  ///    bit i+D     bit i   interpretation
  ///         0        1    ith variable decreasing  
  ///         1        0    ith variable increasing
  ///         0        0    ith variable can either increase or decrease
  ///   Note the limitation to 32 dimensions. Here D is the number of
  ///   dimensions.
  uint64_t
  label ( void ) const;

  /// dimension
  ///   Return number of dimensions
  uint64_t
  dimension ( void ) const;

  /// poset
  ///   Return underlying poset
  Poset
  poset ( void ) const;

  /// event
  ///   Return event variable associated with vertex
  uint64_t
  event ( uint64_t v ) const;

  /// stringify
  ///   Return a JSON description
  std::string
  stringify ( void ) const;

  /// parse
  ///   Initialize from a JSON description
  ///   Format: a JSON object of the form
  ///     { "poset" : <poset-JSON>, "events" : <events-JSON>, 
  ///       "label" : <label-JSON>, "dimension" : <dimension-JSON> }
  ///   Here, <poset-JSON> is a JSON representation the Poset class can parse
  ///         <events-JSON> is an array of variable indices correspond to poset elements in order
  ///         <label-JSON> is an integer label represent the final state after the last match (see label method)
  ///         <dimension-JSON> is an integer giving the number of variables
  void
  parse ( std::string const& str );

private:
  std::shared_ptr<Pattern_> data_; 
};

struct Pattern_ {
  Poset poset_;
  std::vector<uint64_t> events_;
  uint64_t label_;
  uint64_t dimension_;
};

/// Python Bindings

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
namespace py = pybind11;

inline void
PatternBinding (py::module &m) {
  py::class_<Pattern, std::shared_ptr<Pattern>>(m, "Pattern")
    .def(py::init<>())
    .def(py::init<Poset const&, std::vector<uint64_t> const&, uint64_t, uint64_t >())
    .def("load", &Pattern::load)
    .def("label", &Pattern::label)
    .def("dimension", &Pattern::dimension)
    .def("poset", &Pattern::poset)
    .def("event", &Pattern::event)
    .def("stringify", &Pattern::stringify)
    .def("parse", &Pattern::parse);
}
