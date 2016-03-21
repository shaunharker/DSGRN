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
PatternGraph ( void ) const {
  return data_ -> pg_;
}

SearchGraph const& MatchingGraph::
SearchGraph ( void ) const {
  return data_ ->  sg_;
}

std::vector<uint64_t> const& MatchingGraph::
adjacencies ( uint64_t v ) const {
/*
def matching_graph_adjacencies(vertex, domaingraph, patterngraph):
  # Empty list to store results
  result = [];
  # A matching graph vertex is a (domain, position) pair
  domain = vertex[0]; position = vertex[1];
  for nextdomain in domaingraph.adjacencies(domain):
    switching_variable = domaingraph.switching(domain, nextdomain)
    # Handle "intermediate" matches:
    current_symbol = patterngraph.label(position)[switching_variable]
    next_symbol = domaingraph.label(nextdomain)[switching_variable]
    if match(current_symbol, next_symbol): result . append ( [ nextdomain, position ] )
    # Handle "extremal" matches:
    next_position = patterngraph.switch(position, switching_variable)
    if next_position: result . append ( [ nextdomain, nextposition] )
  return result
*/
  return data_ -> digraph_ . adjacencies ( v );
}

uint64_t MatchingGraph::
domain ( Vertex const& v ) const {
  return v . first;
}

uint64_t MatchingGraph::
position ( Vertex const& v ) const {
  return v . second;
}

uint64_t MatchingGraph::
vertex ( uint64_t domain, uint64_t position ) const {
  return Vertex ( {domain, position} );
}
