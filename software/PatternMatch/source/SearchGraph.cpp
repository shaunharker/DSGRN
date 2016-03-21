/// SearchGraph.h
/// Shaun Harker and Bree Cummins
/// 2016-03-20

#include "SearchGraph.h"

SearchGraph::
SearchGraph ( void ) {
  data_ . reset ( new SearchGraph_ );
}

SearchGraph::
SearchGraph ( DomainGraph const& dg, uint64_t morse_set_index ) {
  assign ( dg, morse_set_index );
}

void SearchGraph::
assign ( DomainGraph const& dg, uint64_t morse_set_index ) {
  data_ . reset ( new SearchGraph_ );
  // TODO
}

uint64_t SearchGraph::
size ( void ) const {
  return data_ -> digraph_ . size ();
}

std::vector<uint8_t> SearchGraph::
label ( uint64_t v ) const {
  return data_ -> labels_ [ v ];
}

std::vector<uint64_t> const& SearchGraph::
adjacencies ( uint64_t v ) const {
  return data_ -> digraph_ . adjacencies ( v );
}

uint64_t SearchGraph::
switching ( uint64_t source, uint64_t target ) const {
  return data_ -> switching_ [ source ] [ target ];
}
