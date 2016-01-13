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
  // construct the reachability matrix using warshall algorithm
  // and also precompute some quantities
  // 1^T A and A 1 with A the reachability matrix
  warshall();
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

  // make a copy (necessary?)
  transitiveReduction_ = data_ -> adjacencies_;
  // Pre-compute some quantities
  sumRowTransitiveReduction();
  sumColumnTransitiveReduction();
  //
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

// If B is the transitive reduction matrix,
// we compute 1^T B
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

// If B is the transitive reduction matrix
// we compute B 1, with 1 is the one vector
// i.e., add up the columns (component wise) = list adjacency . size()
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

INLINE_IF_HEADER_ONLY void Poset::
warshall ( void ) {
  uint64_t N = size();
  //
  bool A[N][N];
  //
  for ( uint64_t i=0; i<N; ++i ) {
    for ( uint64_t j=0; j<N; ++j ) {
      A[i][j] = false;
    }
  }
  // Construct the adjacency matrix
  for ( uint64_t i=0; i<data_->adjacencies_.size(); ++i ) {
    for ( auto u : data_ -> adjacencies_ [ i ] ) {
      A [ i ] [ u ] = true;
    }
  }
  // Algorithm
  for ( uint64_t k=0; k<N; ++k ) {
    for ( uint64_t i=0; i<N; ++i ) {
      for ( uint64_t j=0; j<N; ++j ) {
        A [ i ] [ j ] = A[i][j] || (A[i][k] && A[k][j]);
      }
    }
  }
  // Fill the reachability matrix
  // and precompute some quantities
  reachability_ . resize ( N );
  sumRowReachability_ . resize ( N, 0 );
  sumColumnReachability_ . resize ( N, 0 );
  for ( uint64_t i=0; i<N; ++i ) {
    reachability_ [ i ] . resize ( N, 0 );
  }
  for ( uint64_t i=0; i<N; ++i ) {
    // need to go through half of the matrix
    for ( uint64_t j=i; j<N; ++j ) {
      if ( A[i][j] ) {
        reachability_[i][j] = 1;
        ++sumRowReachability_[j]; // Sum of the rows component wise : 1^T A
        ++sumColumnReachability_[i]; // Sum of the columns component wise : A 1
      }
    }
  }
}


#endif
