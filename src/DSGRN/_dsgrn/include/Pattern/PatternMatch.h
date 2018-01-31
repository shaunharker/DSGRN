/// PatternMatch.h
/// MIT LICENSE
/// Shaun Harker and Bree Cummins
/// 2016-03-19

#pragma once

#include "common.h"

#include "Pattern/MatchingGraph.h"

/// QueryCycleMatch
///   Determine if there is a cycle match
bool
QueryCycleMatch ( MatchingGraph const& mg );

/// QueryCycleMatch
///   Determine if there is a path match
bool
QueryPathMatch ( MatchingGraph const& mg );

/// CycleMatch
///   Return a cycle match (or else empty if none exists)
std::vector<MatchingGraph::Vertex>
CycleMatch ( MatchingGraph const& mg );

/// PathMatch
///   Return a path match (or else empty if none exists)
std::vector<MatchingGraph::Vertex>
PathMatch ( MatchingGraph const& mg );

/// FindPath
///   Given a MatchingGraph "mg", a starting vertex "start", and a predicate on pairs (u,v),
///   find a path from start to a vertex v such that match(start, v) evaluates to true
/// TODO: a little wonky, since if start is set a-priori, why two arguments on match?
///       the reason is that ideally there would be no "start" argument and it would search for
///       any match. But this is inefficient if very few "start" values ever give match(start, *) = true
///       In particular, a nicer interface could be to provide match as is, and an _optional_ argument
///       which gave { u : there exists v such that match(u,v) } in the form of some kind of container
///       (with iterates generated on the fly perhaps rather than stored a-priori). However that's a
///       lot of work in C++ so currently the interface just gives a single "start" and the calling code
///       must run FindPath in a for loop.
std::vector<MatchingGraph::Vertex>
FindPath ( MatchingGraph const& mg, 
           MatchingGraph::Vertex const& start, 
           std::function<bool(MatchingGraph::Vertex const&, MatchingGraph::Vertex const&)> const& match );

// /// CountCycleMatches
// ///   Return the number of cycle matches
// uint64_t
// CountCycleMatches ( MatchingGraph const& mg );

// /// CountPathMatches
// ///   Return the number of path matches
// uint64_t
// CountPathMatches ( MatchingGraph const& mg );

/// Debugging

/// ExplainCycleMatch
///   Return a path through the MatchingGraph corresponding 
///   to a cycle match
std::string
ExplainCycleMatch ( MatchingGraph const& mg );

/// Python Bindings

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
namespace py = pybind11;

inline
void PatternMatchBinding(py::module &m) {
  m.def("QueryCycleMatch", &QueryCycleMatch);
  m.def("QueryPathMatch", &QueryPathMatch);
  m.def("CycleMatch", &CycleMatch);
  m.def("PathMatch", &PathMatch);
  m.def("FindPath", &FindPath);
}
