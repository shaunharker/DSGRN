/// Poset.hpp
/// Shaun Harker
/// 2015-05-15

#ifndef DSGRN_POSET_HPP
#define DSGRN_POSET_HPP

#ifndef INLINE_IF_HEADER_ONLY
#define INLINE_IF_HEADER_ONLY
#endif

#include "Poset.h"

INLINE_IF_HEADER_ONLY void Poset::
reduction ( void ) {
  // Algorithm: Remove self-edges, and remove edges which 
  // can be given by a double-hop
  // (Assumes the vertices are topologically sorted.)
  // (Assumes original state is transitively closed.)
  uint64_t N = size ();
  for ( uint64_t u = 0; u < N; ++ u ) {
    std::unordered_set<uint64_t> double_hop;
    double_hop . insert ( u );
    for ( uint64_t v : data_ ->adjacencies_ [ u ] ) {
      if ( u == v ) continue;
      for ( uint64_t w : data_ ->adjacencies_ [ v ] ) {
        if ( v == w ) continue;
        double_hop . insert ( w );
      }
    }
    auto removal_predicate = [&] (uint64_t v) { 
      return double_hop . count ( v ) ? true : false;
    };
    auto it = std::remove_if ( data_ ->adjacencies_[u].begin(), 
                               data_ ->adjacencies_[u].end(), 
                               removal_predicate );
    data_ -> adjacencies_[u].erase ( it, data_ ->adjacencies_[u].end() );
  } 
}
#endif
