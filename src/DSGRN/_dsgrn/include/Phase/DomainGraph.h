/// DomainGraph.h
/// Shaun Harker
/// 2015-05-24

#pragma once

#include "common.h"
#include "Parameter/Parameter.h"
#include "Graph/Digraph.h"
#include "Dynamics/Annotation.h"
#include "Graph/Components.h"

struct DomainGraph_;

class DomainGraph : public TypedObject {
public:

  virtual std::string type (void) const final { return "DomainGraph"; }

  /// constructor
  DomainGraph ( void );

  /// DomainGraph
  ///   Construct based on parameter and network
  DomainGraph ( Parameter const& parameter );

  /// assign
  ///   Construct based on parameter and network
  void
  assign ( Parameter const& parameter );

  /// parameter
  ///   Return underlying parameter
  Parameter const
  parameter ( void ) const;

  /// digraph
  ///   Return underlying digraph
  Digraph const
  digraph ( void ) const;  

  /// dimension
  ///   Return dimension of phase space
  uint64_t
  dimension ( void ) const;

  /// coordinates
  ///   Given a domain vertex, express the domain
  ///   in coordinates (n1, n2, ..., nd), where
  ///   ni is the number of thresholds the domain 
  ///   is above in the ith dimension
  std::vector<uint64_t>
  coordinates ( uint64_t domain ) const;
  
  /// label
  ///   Given a domain, return a 64-bit integer 
  ///   which indicates whether each wall is an entrance
  ///   wall or an absorbing wall. The information is
  ///   encoded bitwise for the 2*d walls in the following pattern:
  ///     (from least significant to most significant bits)
  ///     left-0, left-1, left-2, ..., left-(d-1),
  ///     right-0, right-1, ... right(d-1)
  ///   A bit of 0 means entrance and 1 means absorbing.
  uint64_t 
  label ( uint64_t domain ) const;

  /// label
  ///   Given (u,u), return 0.
  ///   Given a pair of adjacent domains (source, target), return a 64-bit integer 
  ///   which indicates the search graph label for the edge.
  ///   For bits i+D and i,  00 means -  (both minima and maxima ruled out)
  ///                        10 means m  (maxima in i ruled out)
  ///                        01 means M  (minima in i ruled out)
  ///                        11 means *  (anything may happen (never occurs)) 
  uint64_t 
  label ( uint64_t source, uint64_t target ) const;

  /// direction
  ///   Given adjacent domains, return the variable
  ///   in which they differ. Given the same domain 
  ///   as both source and target, return dimension().
  ///   Undefined behavior for non-adjacent domains.
  uint64_t
  direction ( uint64_t source, uint64_t target ) const;

  /// regulator
  ///   Given adjacent domains, return the variable
  ///   being regulated between them. Given the same domain 
  ///   as both source and target, return dimension().
  ///   Undefined behavior for non-adjacent domains.
  uint64_t
  regulator ( uint64_t source, uint64_t target ) const;

  /// annotate
  ///   Given a collection of vertices, return an
  ///   annotation describing them
  Annotation const
  annotate ( Component const& vertices ) const;

  /// graphviz
  ///   Return a graphviz representation of the domain graph
  std::string
  graphviz ( void ) const;

  /// operator <<
  ///   Emit data to stream in graphviz format
  friend std::ostream& operator << ( std::ostream& stream, DomainGraph const& dg );

private:
  std::shared_ptr<DomainGraph_> data_;
};

struct DomainGraph_ {
  uint64_t dimension_;
  Digraph digraph_;
  Parameter parameter_;
  std::vector<uint64_t> labelling_;
  std::unordered_map<uint64_t,uint64_t> direction_;
};

/// Python Bindings

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
namespace py = pybind11;

inline void
DomainGraphBinding (py::module &m) {
  py::class_<DomainGraph, std::shared_ptr<DomainGraph>, TypedObject>(m, "DomainGraph")
    .def(py::init<>())
    .def(py::init<Parameter const&>())
    // TODO: increments
    .def("parameter", &DomainGraph::parameter)
    .def("digraph", &DomainGraph::digraph)
    .def("dimension", &DomainGraph::dimension)
    .def("coordinates", &DomainGraph::coordinates)
    .def("label", (uint64_t(DomainGraph::*)(uint64_t)const)&DomainGraph::label)
    .def("label", (uint64_t(DomainGraph::*)(uint64_t,uint64_t)const)&DomainGraph::label)
    .def("direction", &DomainGraph::direction)
    .def("regulator", &DomainGraph::regulator)
    .def("annotate", &DomainGraph::annotate)
    .def("graphviz", &DomainGraph::graphviz)
    .def("__str__", [](DomainGraph * lp){ std::stringstream ss; ss << *lp; return ss.str(); });

}
