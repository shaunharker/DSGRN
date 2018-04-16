/// LogicParameter.h
/// Shaun Harker
/// 2015-05-24

#pragma once

#include "common.h"

struct LogicParameter_;

class LogicParameter {
public:
  /// LogicParameter
  LogicParameter ( void );

  /// LogicParameter
  LogicParameter ( uint64_t n, uint64_t m, std::string const& hex );

  /// assign
  ///   Assign data
  void
  assign ( uint64_t n, uint64_t m, std::string const& hex );

  /// operator ()
  ///   Return true if input combination drives the output
  ///   to the right, and false if it drives it to the left
  bool
  operator () ( std::vector<bool> const& input_combination, uint64_t output ) const;

  /// operator ()
  ///   Version of operator () where bit has already been computed
  bool
  operator () ( uint64_t bit ) const;

  /// bin
  ///   Return bin number corresponding to input combination
  ///   (uses uint64_t, assumes there are 64 or fewer inputs)
  uint64_t
  bin ( uint64_t input_combination ) const;

  /// stringify
  ///   Return a JSON-style string
  ///    of form [n,m,"hex"]
  std::string
  stringify ( void ) const;

  /// parse
  ///   Initialize from a JSON-style string
  void
  parse ( std::string const& str );

  /// numInputs()
  ///   Returns n
  uint64_t
  numInputs() const;

  /// numOutputs()
  ///   Returns m
  uint64_t
  numOutputs() const;

  /// hex ()
  ///   Return the hex code of the LogicParameter
  std::string const &
  hex ( void ) const;

  /// adjacencies ()
  ///   Return the adjacent LogicParameters
  std::vector<LogicParameter>
  adjacencies ( void ) const;

  /// operator ==
  ///   Equality comparison
  bool
  operator == ( LogicParameter const& rhs ) const;
  
  /// operator <<
  ///   Output debug data to stream
  friend std::ostream& operator << ( std::ostream& stream, LogicParameter const& p );

private:
  std::shared_ptr<LogicParameter_> data_;
};

struct LogicParameter_ {
  std::string hex_;
  std::vector<bool> comp_;
  uint64_t n_;
  uint64_t m_;
};

/// Python Bindings

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
namespace py = pybind11;

inline void
LogicParameterBinding (py::module &m) {
  py::class_<LogicParameter, std::shared_ptr<LogicParameter>>(m, "LogicParameter")
    .def(py::init<>())
    .def(py::init<uint64_t, uint64_t, std::string const&>())
    .def("__call__", (bool(LogicParameter::*)( std::vector<bool> const&, uint64_t)const)&LogicParameter::operator())
    .def("bin", &LogicParameter::bin)
    .def("stringify", &LogicParameter::stringify)
    .def("parse", &LogicParameter::parse)
    .def("hex", &LogicParameter::hex)
    .def("adjacencies", &LogicParameter::adjacencies)
    .def("numInputs", &LogicParameter::numInputs )
    .def("numOutputs", &LogicParameter::numOutputs )    
    .def("__eq__", &LogicParameter::operator==)
    .def("__str__", [](LogicParameter * lp){ std::stringstream ss; ss << *lp; return ss.str(); })
    .def(py::pickle(
    [](LogicParameter const& p) { // __getstate__
        /* Return a tuple that fully encodes the state of the object */
        return py::make_tuple(p.numInputs(), p.numOutputs(), p.hex());
    },
    [](py::tuple t) { // __setstate__
        if (t.size() != 3)
            throw std::runtime_error("Unpickling LogicParameter object: Invalid state!");
        /* Create a new C++ instance */
        return LogicParameter(t[0].cast<uint64_t>(), t[1].cast<uint64_t>(), t[2].cast<std::string>() );
    }));
}
