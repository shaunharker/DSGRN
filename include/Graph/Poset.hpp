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
  //
  reachability_ = data_ -> adjacencies_;

  // constructReachabilityMatrix();
  //
  sumRowReachability();
  sumColumnReachability();
  //
  // for ( uint64_t i=0; i<N; ++i ) {
  //   std::cout << "#ancestors for " << i << " is " << numberOfAncestors(i) << "\n";
  //   std::cout << "#descendants for " << i << " is " << numberOfDescendants(i) << "\n";
  // }
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

  // constructTransitiveReductionMatrix();
  transitiveReduction_ = data_ -> adjacencies_;
  //
  sumRowTransitiveReduction();
  sumColumnTransitiveReduction();
  //
  // for ( uint64_t i=0; i<N; ++i ) {
  //   std::cout << "#parents for " << i << " is " << numberOfParents(i) << "\n";
  //   std::cout << "#children for " << i << " is " << numberOfChildren(i) << "\n";
  // }

}

INLINE_IF_HEADER_ONLY uint64_t Poset::
numberOfAncestors ( const uint64_t & i ) const {
  return sumRowReachability_ [ i ];
}

INLINE_IF_HEADER_ONLY uint64_t Poset::
numberOfDescendants ( const uint64_t & i ) const {
  return sumColumnReachability_ [ i ];
}

INLINE_IF_HEADER_ONLY uint64_t Poset::
numberOfParents ( const uint64_t & i ) const {
  return sumRowTransitiveReduction_ [ i ];
}

INLINE_IF_HEADER_ONLY uint64_t Poset::
numberOfChildren ( const uint64_t & i ) const {
  return sumColumnTransitiveReduction_ [ i ];
}


// If A is the reachability matrix,
// we compute 1^T A. Add up row component wise.
// We use the adjacency list instead of the matrix
INLINE_IF_HEADER_ONLY void Poset::
sumRowReachability ( void ) {
  //
  sumRowReachability_ . resize ( size() , 0 );
  //
  for ( auto u : reachability_ ) {
    for ( auto v : u ) {
      ++ sumRowReachability_ [ v ];
    }
  }
}

// If A is the reachability matrix,
// we compute A 1, with 1 is the one vector
// add up the columns component wise
// use the adjacency list instead of the matrix
INLINE_IF_HEADER_ONLY void Poset::
sumColumnReachability ( void ) {
  // Number of nodes
  uint64_t N = size();
  //
  sumColumnReachability_ . resize ( N, 0 );
  //
  for ( uint64_t i=0; i<N; ++i ) {
    sumColumnReachability_ [ i ] = reachability_ [ i ] . size();
  }
}

// If B is the transitive reduction matrix,
// we compute 1^T A
INLINE_IF_HEADER_ONLY void Poset::
sumRowTransitiveReduction ( void ) {
  //
  sumRowTransitiveReduction_ . resize ( size() , 0 );
  //
  for ( auto u : transitiveReduction_ ) {
    for ( auto v : u ) {
      ++ sumRowTransitiveReduction_[v];
    }
  }
}

// If A is the reachability matrix,
// we compute A 1, with 1 is the one vector
// add up the columns component wise
INLINE_IF_HEADER_ONLY void Poset::
sumColumnTransitiveReduction ( void ) {
  // Number of nodes
  uint64_t N = size();
  //
  sumColumnTransitiveReduction_ . resize ( N, 0 );
  //
  for ( uint64_t i=0; i<N; ++i ) {
    sumColumnTransitiveReduction_ [ i ] = transitiveReduction_ [ i ] . size();
  }
}




#endif
