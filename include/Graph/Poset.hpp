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
  //
  uint64_t N = size ();

  dataPoset_ -> parentsCount_ . resize ( N );
  //   dataPoset_ -> childrenCount_ . resize ( N );
  //   dataPoset_ -> ancestorsCount_ . resize ( N );
  //   dataPoset_ -> descendantsCount_ . resize ( N );
  //   computeNumberOfAncestors();
  //   computeNumberOfDescendants();


  //
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

  computeNumberOfParents();
  computeNumberOfChildren();

}

INLINE_IF_HEADER_ONLY uint64_t Poset::
numberOfAncestors ( const uint64_t & i ) const {
  return dataPoset_ -> ancestorsCount_ [ i ];
}

INLINE_IF_HEADER_ONLY uint64_t Poset::
numberOfDescendants ( const uint64_t & i ) const {
  return dataPoset_ -> descendantsCount_ [ i ];
}

INLINE_IF_HEADER_ONLY uint64_t Poset::
numberOfParents ( const uint64_t & i ) const {
  return dataPoset_ -> parentsCount_ [ i ];
}

INLINE_IF_HEADER_ONLY uint64_t Poset::
numberOfChildren ( const uint64_t & i ) const {
  return dataPoset_ -> childrenCount_ [ i ];
}

INLINE_IF_HEADER_ONLY void Poset::
computeNumberOfAncestors ( void ) const {
  uint64_t N = size();
  for ( uint64_t i=0; i<N; ++i ) {
    for ( auto u : dataPoset_ -> reachability_ [ i ] ) {
      ++ dataPoset_ -> ancestorsCount_ [ u ];
    }
  }
}

INLINE_IF_HEADER_ONLY void Poset::
computeNumberOfDescendants ( void ) const {
  uint64_t N = size();
  for ( uint64_t i=0; i<N; ++i ) {
    dataPoset_ -> descendantsCount_ [ i ] =  dataPoset_ -> reachability_ [ i ] . size();
  }
}

INLINE_IF_HEADER_ONLY void Poset::
computeNumberOfParents ( void ) const {
  uint64_t N = size();
  for ( uint64_t i=0; i<N; ++i ) {
    for ( auto u : data_ -> adjacencies_ [ i ] ) {
      ++ dataPoset_ -> parentsCount_ [ u ];
    }
  }
}

INLINE_IF_HEADER_ONLY void Poset::
computeNumberOfChildren ( void ) const {
  uint64_t N = size();
  for ( uint64_t i=0; i<N; ++i ) {
    dataPoset_ -> childrenCount_ [ i ] =  data_ -> adjacencies_ [ i ] . size();
  }
}

INLINE_IF_HEADER_ONLY bool Poset::
reachable ( const uint64_t & n, const uint64_t & m ) const {
  for ( auto u : data_ -> adjacencies_ [ n ] ) {
    if ( u == m ) {
      return true;
    }
  }
  return false;
}

#endif
