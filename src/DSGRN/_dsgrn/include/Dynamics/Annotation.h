/// Annotation.h
/// Shaun Harker
/// 2015-05-15

#pragma once

#include "common.h"

struct Annotation_;

/// Annotation
///   A lightweight string container
///   used to annotate Morse Graphs
class Annotation {
public:
  /// iterator
  ///   STL-style iterator
  typedef std::vector<std::string>::const_iterator iterator;

  /// constructor
  Annotation ( void );

  /// size
  ///   Return the number of annotations
  uint64_t 
  size ( void ) const;

  /// begin
  ///   Return start iterator
  iterator
  begin ( void ) const;

  /// end
  ///   Return end iterator
  iterator 
  end ( void ) const;

  /// operator []
  ///   Random access to contents
  std::string const&
  operator [] ( uint64_t i ) const;

  /// append
  ///   Add more annotations
  void
  append ( std::string const& label );

  /// stringify
  ///   Return a JSON description
  std::string
  stringify ( void ) const;

  /// parse
  ///   Initialize from a JSON description
  Annotation &
  parse ( std::string const& str );

  /// operator <<
  ///   Output list of annotations
  friend std::ostream& operator << ( std::ostream& stream, Annotation const& a );
private:
  std::shared_ptr<Annotation_> data_;
};

struct Annotation_ {
  std::vector<std::string> annotations_;
};

/// Python Bindings

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
namespace py = pybind11;

inline void
AnnotationBinding(py::module &m) {
  py::class_<Annotation, std::shared_ptr<Annotation>>(m, "Annotation")
    .def(py::init<>())
    .def("size", &Annotation::size)
    .def("begin", &Annotation::begin)
    .def("end", &Annotation::end)
    .def("__iter__", [](Annotation const& v) {
        return py::make_iterator(v.begin(), v.end());
      }, py::keep_alive<0, 1>())
    .def("__getitem__", [](Annotation const& v, uint64_t key) {
      return v[key];
    })
    .def("append", &Annotation::append)
    .def("stringify", &Annotation::stringify)
    .def("parse", &Annotation::parse)
    .def("str", [](Annotation * a){ std::stringstream ss; ss << *a; return ss.str(); })
    .def(py::pickle(
    [](Annotation const& p) { // __getstate__
        /* Return a tuple that fully encodes the state of the object */
        return py::make_tuple(p.stringify());
    },
    [](py::tuple t) { // __setstate__
        if (t.size() != 1)
            throw std::runtime_error("Unpickling Annotation object: Invalid state!");
        /* Create a new C++ instance */
        return Annotation().parse(t[0].cast<std::string>());
    }));
}
