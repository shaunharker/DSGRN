/// Poset.hpp
/// Shaun Harker
/// 2015-05-15

#ifndef DSGRN_POSET_HPP
#define DSGRN_POSET_HPP

#ifndef INLINE_IF_HEADER_ONLY
#define INLINE_IF_HEADER_ONLY
#endif

#include "Poset.h"

INLINE_IF_HEADER_ONLY Poset::
Poset ( void ) {
  dataPoset_ . reset ( new Poset_ );
}

INLINE_IF_HEADER_ONLY Poset::
Poset ( std::vector<std::vector<uint64_t>> & adjacencies ):Digraph(adjacencies) {
  dataPoset_ . reset ( new Poset_ );
}


INLINE_IF_HEADER_ONLY void Poset::
reduction ( void ) {
  // Algorithm: Remove self-edges, and remove edges which
  // can be given by a double-hop
  // (Assumes the vertices are topologically sorted.)
  // (Assumes original state is transitively closed.)
  //
  uint64_t N = size ();
  //
  dataPoset_ -> reachability_ = data_ -> adjacencies_;
  dataPoset_ -> parentsCount_ . resize ( N );
  dataPoset_ -> childrenCount_ . resize ( N );
  dataPoset_ -> ancestorsCount_ . resize ( N );
  dataPoset_ -> descendantsCount_ . resize ( N );
  _computeNumberOfAncestors();
  _computeNumberOfDescendants();
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
  //
  _computeNumberOfParents();
  _computeNumberOfChildren();
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
_computeNumberOfAncestors ( void ) const {
  uint64_t N = size();
  for ( uint64_t i=0; i<N; ++i ) {
    for ( auto u : dataPoset_ -> reachability_ [ i ] ) {
      ++ dataPoset_ -> ancestorsCount_ [ u ];
    }
  }
}

INLINE_IF_HEADER_ONLY void Poset::
_computeNumberOfDescendants ( void ) const {
  uint64_t N = size();
  for ( uint64_t i=0; i<N; ++i ) {
    dataPoset_ -> descendantsCount_ [ i ] =  dataPoset_ -> reachability_ [ i ] . size();
  }
}

INLINE_IF_HEADER_ONLY void Poset::
_computeNumberOfParents ( void ) const {
  uint64_t N = size();
  for ( uint64_t i=0; i<N; ++i ) {
    for ( auto u : data_ -> adjacencies_ [ i ] ) {
      ++ dataPoset_ -> parentsCount_ [ u ];
    }
  }
}

INLINE_IF_HEADER_ONLY void Poset::
_computeNumberOfChildren ( void ) const {
  uint64_t N = size();
  for ( uint64_t i=0; i<N; ++i ) {
    dataPoset_ -> childrenCount_ [ i ] =  data_ -> adjacencies_ [ i ] . size();
  }
}


INLINE_IF_HEADER_ONLY bool Poset::
reachable ( const uint64_t & n, const uint64_t & m ) const {
  if ( n < m ) {
    for ( auto u : data_ -> adjacencies_ [ n ] ) {
      if ( u == m ) {
        return true;
      }
    }
  }
  return false;
}


INLINE_IF_HEADER_ONLY Poset Poset::
reorder ( const std::vector<uint64_t> & ordering ) const {
  /// Reconstruct the list adjacencies from ordering
  std::vector< std::vector<uint64_t> > newAdjacencies;
  /// ordering [ 3 ] = 11 means node label 3 should become 11
  ///
  uint64_t N = size();
  newAdjacencies . resize ( N );
  ///
  for ( uint64_t i=0; i<N; ++i ) {
    uint64_t newVertex1 = ordering [ i ];
    for ( auto u : data_ -> adjacencies_ [ i ] ) {
      uint64_t newVertex2 = ordering [ u ];
      newAdjacencies [ newVertex1 ] . push_back ( newVertex2 );
    }
  }
  ///
  Poset newPoset ( newAdjacencies );
  /// to recompute ancestors,descendants,parents,children
  newPoset . reduction ();
  ///
  return newPoset;
}



INLINE_IF_HEADER_ONLY void Poset::
transitiveClosure ( void ) {
  ///
  /// Warshall algorithm
  ///
  uint64_t N = size();
  bool A[N][N];
  ///
  for ( uint64_t i=0; i<N; ++i ) {
    for ( uint64_t j=0; j<N; ++j ) {
      A[i][j] = false;
    }
  }
  /// Construct the adjacency matrix
  for ( uint64_t i=0; i<data_->adjacencies_.size(); ++i ) {
    for ( auto u : data_ -> adjacencies_ [ i ] ) {
      A [ i ] [ u ] = true;
    }
    A [ i ] [ i ] = true; /// a node can always reach itself
  }
  /// Algorithm
  for ( uint64_t k=0; k<N; ++k ) {
    for ( uint64_t i=0; i<N; ++i ) {
      for ( uint64_t j=0; j<N; ++j ) {
        A [ i ] [ j ] = A[i][j] || (A[i][k] && A[k][j]);
      }
    }
  }
  ///
  /// rewrite the adjacency list
  data_ -> adjacencies_ . clear();
  data_ -> adjacencies_ . resize ( N );
  //
  for ( uint64_t i=0; i<N; ++i ) {
    /// need to go through half of the matrix only
    for ( uint64_t j=i; j<N; ++j ) {
      if ( A[i][j] ) {
        data_ -> adjacencies_ [ i ] . push_back ( j );
      }
    }
  }
}


#endif
