/// MatchingGraph.cpp
/// Shaun Harker and Bree Cummins
/// 2016-03-20

#include "MatchingGraph.h"


MatchingGraph::
MatchingGraph ( void ) {
  data_ . reset ( new MatchingGraph_ );
}

MatchingGraph::
MatchingGraph ( PatternGraph const& pg, SearchGraph const& sg ) {
  assign ( pg, sg );
}

void MatchingGraph::
assign ( PatternGraph const& pg, SearchGraph const& sg ) {
  data_ . reset ( new MatchingGraph_ );
  // TODO
}

PatternGraph const& MatchingGraph::
PatternGraph ( void ) const {
  return data_ -> pg_;
}

SearchGraph const& MatchingGraph::
SearchGraph ( void ) const {
  return data_ ->  sg_;
}

uint64_t MatchingGraph::
size ( void ) const {
  return data_ -> digraph_ . size ();
}

std::vector<uint64_t> const& MatchingGraph::
adjacencies ( uint64_t v ) const {
  return data_ -> digraph_ . adjacencies ( v );
}

uint64_t MatchingGraph::
domain ( uint64_t v ) const {
  return data_ -> domain_ [ v ];
}

uint64_t MatchingGraph::
position ( uint64_t v ) const {
  return data_ -> position_ [ v ];
}

uint64_t MatchingGraph::
vertex ( uint64_t domain, uint64_t position ) const {
  return data_ -> vertex_[{domain, position}];
}
