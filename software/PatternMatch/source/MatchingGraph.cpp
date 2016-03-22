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
  data_ -> pg_ = pg;
  data_ -> sg_ = sg;
}

PatternGraph const& MatchingGraph::
patterngraph ( void ) const {
  return data_ -> pg_;
}

SearchGraph const& MatchingGraph::
searchgraph ( void ) const {
  return data_ ->  sg_;
}

std::vector<MatchingGraph::Vertex> MatchingGraph::
adjacencies ( Vertex const& v ) const {
  std::vector<Vertex> result;
  uint64_t const& domain = v . first;
  uint64_t const& position = v . second;
  for ( auto nextdomain : searchgraph() . adjacencies(domain) ) {
    // Check for intermediate match
    if ( _match ( searchgraph().label(nextdomain), patterngraph().label(position) ) ) {
      result . push_back ( Vertex(nextdomain, position) );
    }
    // Check for extremal match
    // Determine what variable can have a min/max event 
    uint64_t variable = searchgraph() . event ( domain, nextdomain );
    if ( variable == -1 ) continue;
    // Find successor in pattern graph which consumes event
    uint64_t nextposition = patterngraph() . consume ( position, variable );
    if ( nextposition == -1 ) continue;
    result . push_back ( Vertex(nextdomain, nextposition) );
  }
  return result;
}

uint64_t MatchingGraph::
domain ( Vertex const& v ) const {
  return v . first;
}

uint64_t MatchingGraph::
position ( Vertex const& v ) const {
  return v . second;
}

MatchingGraph::Vertex MatchingGraph::
vertex ( uint64_t domain, uint64_t position ) const {
  return Vertex ( {domain, position} );
}

bool MatchingGraph::
_match ( uint64_t search_label, uint64_t pattern_label ) const {
  return (pattern_label & search_label) == pattern_label;
}
