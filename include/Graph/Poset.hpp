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
  constructReachabilityMatrix();
  //
  sumRowReachability();
  sumColumnReachability();
  //
  for ( uint64_t i=0; i<N; ++i ) {
    std::cout << "#ancestors for " << i << " is " << numberOfAncestors(i) << "\n";
    std::cout << "#descendants for " << i << " is " << numberOfDescendants(i) << "\n";
  }
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

  constructTransitiveReductionMatrix();
  //
  sumRowTransitiveReduction();
  sumColumnTransitiveReduction();
  //
  for ( uint64_t i=0; i<N; ++i ) {
    std::cout << "#parents for " << i << " is " << numberOfParents(i) << "\n";
    std::cout << "#children for " << i << " is " << numberOfChildren(i) << "\n";
  }

}

INLINE_IF_HEADER_ONLY void Poset::
constructReachabilityMatrix ( void ) {
  uint64_t N = size ( );
  reachabilityMatrix_ . resize ( N );
  for ( uint64_t i=0; i<N; ++i ) {
    reachabilityMatrix_[i] . resize ( N, 0 );
  }

  // A[i,j] = 1 iff p_i < p_j
  for ( uint64_t i=0; i<N; ++i ) {
    for ( uint64_t j : data_ -> adjacencies_[i] ) {
      if ( i < j ) {
        reachabilityMatrix_[i][j] = 1;
      } else {
        reachabilityMatrix_[j][i] = 1;
      }
    }
  }

  // DEBUG
  std::cout << "Reachability matrix\n";
  for ( auto u : reachabilityMatrix_ ) {
    for ( auto v : u ) {
      std::cout << v << " ";
    }
    std::cout << "\n";
  }
  // END DEBUG

}

INLINE_IF_HEADER_ONLY void Poset::
constructTransitiveReductionMatrix ( void ) {
  uint64_t N = size ( );
  transitiveReductionMatrix_ . resize ( N );
  for ( uint64_t i=0; i<N; ++i ) {
    transitiveReductionMatrix_[i] . resize ( N, 0 );
  }

  // A[i,j] = 1 iff p_i < p_j
  for ( uint64_t i=0; i<N; ++i ) {
    for ( uint64_t j : data_ -> adjacencies_[i] ) {
      if ( i < j ) {
        transitiveReductionMatrix_[i][j] = 1;
      } else {
        transitiveReductionMatrix_[j][i] = 1;
      }
    }
  }

  // DEBUG
  std::cout << "Transitive Reduction matrix\n";
  for ( auto u : transitiveReductionMatrix_ ) {
    for ( auto v : u ) {
      std::cout << v << " ";
    }
    std::cout << "\n";
  }
  // END DEBUG

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
// we compute 1^T A
INLINE_IF_HEADER_ONLY void Poset::
sumRowReachability ( void ) {
  // number of rows in the adjacencies matrix
  // or number of nodes
  uint64_t N = size();

  sumRowReachability_ . resize ( N, 0 );
  //
  for ( uint64_t j=0; j<N; ++j ) {
    uint64_t sum = 0;
    for ( uint64_t i=0; i<N; ++i ) {
      sum += reachabilityMatrix_[i][j];
    }
    sumRowReachability_ [ j ] = sum;
  }
}

// If A is the reachability matrix,
// we compute A 1, with 1 is the one vector
// add up the columns component wise
INLINE_IF_HEADER_ONLY void Poset::
sumColumnReachability ( void ) {
  // number of rows in the adjacencies matrix
  // or number of nodes
  uint64_t N = size();
  //
  sumColumnReachability_ . resize ( N, 0 );
  // //
  for ( uint64_t i=0; i<N; ++i ) {
    for ( uint64_t j=0; j<N; ++j ) {
      sumColumnReachability_ [i] += reachabilityMatrix_[i][j];
    }
  }
}

// If B is the transitive reduction matrix,
// we compute 1^T A
INLINE_IF_HEADER_ONLY void Poset::
sumRowTransitiveReduction ( void ) {
  // number of rows in the adjacencies matrix
  // or number of nodes
  uint64_t N = size();

  sumRowTransitiveReduction_ . resize ( N, 0 );
  //
  for ( uint64_t j=0; j<N; ++j ) {
    uint64_t sum = 0;
    for ( uint64_t i=0; i<N; ++i ) {
      sum += transitiveReductionMatrix_[i][j];
    }
    sumRowTransitiveReduction_ [ j ] = sum;
  }
}

// If A is the reachability matrix,
// we compute A 1, with 1 is the one vector
// add up the columns component wise
INLINE_IF_HEADER_ONLY void Poset::
sumColumnTransitiveReduction ( void ) {
  // number of rows in the adjacencies matrix
  // or number of nodes
  uint64_t N = size();
  //
  sumColumnTransitiveReduction_ . resize ( N, 0 );
  // //
  for ( uint64_t i=0; i<N; ++i ) {
    for ( uint64_t j=0; j<N; ++j ) {
      sumColumnTransitiveReduction_ [i] += transitiveReductionMatrix_[i][j];
    }
  }
}




#endif
