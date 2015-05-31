/// StrongComponents.hpp
/// Shaun Harker
/// 2015-05-24

#ifndef DSGRN_STRONGCOMPONENTS_HPP
#define DSGRN_STRONGCOMPONENTS_HPP

#ifndef INLINE_IF_HEADER_ONLY
#define INLINE_IF_HEADER_ONLY
#endif

#include "StrongComponents.h"

// Tarjan's Algorithm
// See http://en.wikipedia.org/wiki/Tarjan's_strongly_connected_components_algorithm
INLINE_IF_HEADER_ONLY Components
StrongComponents ( Digraph const digraph ) {
  Components components;
  uint64_t N = digraph . size ();
  uint64_t n = 0;
  std::vector<std::shared_ptr<std::vector<uint64_t>>> SCCs;
  std::vector<uint64_t> lowlink (N, N);
  std::vector<uint64_t> index (N, N);
  std::vector<bool> committed ( N, false );
  std::vector<uint64_t> S;
  std::function<void(uint64_t)> strongconnect = [&] (uint64_t v) {
    lowlink[v] = index[v] = n ++;
    S . push_back ( v );
    std::vector<uint64_t> const& children = digraph . adjacencies ( v );
    for ( uint64_t child : children ) { 
      if ( index [ child ] == N ) {
        strongconnect ( child );
        lowlink[v] = std::min(lowlink[v], lowlink[child]);
      } else if ( not committed [ child ] ) {
        lowlink[v] = std::min(lowlink[v], index[child]);
      }
    }
    if ( lowlink[v] == index[v] ) {
      std::shared_ptr<std::vector<uint64_t>> SCC ( new std::vector<uint64_t> );
      SCCs . push_back ( SCC );
      while ( 1 ) { 
        uint64_t u = S . back ();
        committed [ u ] = true;
        SCC -> push_back ( u );
        S . pop_back ();
        if ( u == v ) break;
      };
    }
  };
  for ( uint64_t v = 0; v < N; ++ v ) {
    if ( index [ v ] < N ) continue;
    strongconnect ( v );
  } 
  std::reverse ( SCCs . begin (), SCCs . end () );
  components . assign ( SCCs );
  return components;
}

#endif
