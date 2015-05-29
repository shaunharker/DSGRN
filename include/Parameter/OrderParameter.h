/// OrderParameter.h
/// Shaun Harker
/// 2015-05-24

#ifndef DSGRN_ORDERPARAMETER_H
#define DSGRN_ORDERPARAMETER_H

#include <algorithm>
#include <vector>
#include <iostream>
#include <cstdlib>
#include <sstream>

/// class OrderParameter
///   Consider all permutations of m elements,
///   indexed by contiguous integers in lexigraphical
///   order. This class provides methods which 
///   allow one to convert between this indexing
///   and the permutations, along with some convenience 
///   functionality (i.e. applying permutations)
class OrderParameter {
public:
  /// assign (by index)
  ///   Initialize to the kth permutation of n items
  ///   The permutations are ordered lexicographically
  void
  assign ( uint64_t m, uint64_t k );

  /// assign (by permutation)
  ///   Initialize as given permutation
  void
  assign ( std::vector<uint64_t> const& perm );

  /// operator ()
  ///   Apply the permutation to the input
  uint64_t 
  operator () ( uint64_t i ) const;

  /// inverse
  ///   Apply the inverse permuation to input
  uint64_t
  inverse ( uint64_t i ) const;

  /// permutation ()
  ///   Return the underlying permutation
  std::vector<uint64_t> const&
  permutation ( void ) const;

  /// index ()
  ///   Return the index of the permutation
  uint64_t
  index ( void ) const;

  /// size ()
  ///   Return the number of items
  uint64_t
  size ( void ) const;

  /// stringify
  ///   Return a JSON-style string
  ///    of form [p1, p2, p3, ...]
  std::string
  stringify ( void ) const;

  /// parse
  ///   Initialize from a JSON-style string
  void
  parse ( std::string const& str );

  /// operator <<
  ///   Output debug data to stream
  friend std::ostream& operator << ( std::ostream& stream, OrderParameter const& p );

private:
  std::vector<uint64_t> permute_;
  std::vector<uint64_t> inverse_;
  uint64_t k_;
  uint64_t m_;
  static std::vector<uint64_t> _index_to_tail_rep ( uint64_t index );
  static std::vector<uint64_t> _tail_rep_to_perm ( std::vector<uint64_t> const& tail_rep );
  static std::vector<uint64_t> _perm_to_tail_rep ( std::vector<uint64_t> const& perm );
  static uint64_t _tail_rep_to_index ( std::vector<uint64_t> const& tail_rep );
};

inline void OrderParameter::
assign ( uint64_t m, uint64_t k ) {
  k_ = k;
  m_ = m;
  std::vector<uint64_t> tail_rep = _index_to_tail_rep ( k );
  tail_rep . resize ( m_ );
  permute_ = _tail_rep_to_perm ( tail_rep );
  inverse_ . resize ( m_ );
  for ( uint64_t i = 0; i < m_; ++ i ) inverse_[permute_[i]]=i;
}

inline void OrderParameter::
assign ( std::vector<uint64_t> const& perm ) {
  m_ = perm . size ();
  permute_ = perm;
  k_ = _tail_rep_to_index ( _perm_to_tail_rep ( permute_ ) );
  inverse_ . resize ( m_ );
  for ( uint64_t i = 0; i < m_; ++ i ) inverse_[permute_[i]]=i;
}

inline uint64_t OrderParameter::
operator () ( uint64_t i ) const {
  return permute_ [ i ];
}

inline uint64_t OrderParameter::
inverse ( uint64_t i ) const {
  return inverse_ [ i ];
}

inline  std::vector<uint64_t> const& OrderParameter::
permutation ( void ) const {
  return permute_;
}

inline uint64_t OrderParameter::
index ( void ) const {
  return k_;
}

inline uint64_t OrderParameter::
size ( void ) const {
  return m_;
}

inline std::string OrderParameter::
stringify ( void ) const {
  std::stringstream ss;
  ss << *this;
  return ss . str ();
} 

inline void OrderParameter::
parse ( std::string const& str ) {
  std::string s = str;
  auto validcharacter = [] (char c) {
    if ( c >= '0' && c <= '9' ) return true;
    return false;
  };
  for ( char & c : s ) if ( not validcharacter ( c ) ) c = ' ';
  std::stringstream ss ( s );
  uint64_t val;
  permute_ . clear ();
  while ( ss >> val ) permute_ . push_back ( val );
  m_ = permute_ . size ();
  inverse_ . resize ( m_ );
  for ( uint64_t i = 0; i < m_; ++ i ) inverse_[permute_[i]]=i;
}

inline std::ostream& operator << ( std::ostream& stream, OrderParameter const& p ) {
  stream << "[";
  for ( uint64_t i = 0; i < p.m_; ++ i ) {
    if ( i > 0 ) stream << ",";
    stream << p.permute_[i];
  }
  stream << "]";
  return stream;
}

inline std::vector<uint64_t> OrderParameter::
_index_to_tail_rep ( uint64_t index ) {
  std::vector<uint64_t> tail_rep;
  uint64_t i = 1;
  while ( index > 0 ) {
    tail_rep . push_back ( (uint64_t) ( index % (uint64_t) i ) );
    index /= (uint64_t) i;
    ++ i;
  }
  return tail_rep;
}

inline std::vector<uint64_t> OrderParameter::
_tail_rep_to_perm ( std::vector<uint64_t> const& tail_rep ) {
  // Note: This algorithm is suboptimal. It requires O(n^2) time
  //       but there is an O(n log n) algorithm. An optimal 
  //       algorithm requires a counter tree (Red-Black tree which
  //       keeps counts of descendant elements and allows for rank
  //       based queries and insertions)
  std::vector<uint64_t> perm = tail_rep;
  uint64_t m = tail_rep . size ();
  std::reverse ( perm.begin(), perm.end() );
  for ( uint64_t i = 0; i < m; ++ i ) {
    for ( uint64_t j = 0; j < i; ++ j ) {
      if ( perm[m-j-1] >= tail_rep[i] ) ++ perm[m-j-1];
    }
  }
  return perm;
}

inline std::vector<uint64_t> OrderParameter:: 
_perm_to_tail_rep ( std::vector<uint64_t> const& perm ) {
  // Note: This algorithm is suboptimal. It requires O(n^2) time
  //       but there is an O(n log n) algorithm. An optimal 
  //       algorithm requires a counter tree (Red-Black tree which
  //       keeps counts of descendant elements and allows for rank
  //       based queries and insertions)
  uint64_t m = perm . size ();
  std::vector<uint64_t> tail_rep ( m );
  for ( int64_t i = m-1; i > 0; -- i ) {
    for ( int64_t j = i-1; j >= 0; -- j ) {
      if ( perm[m-j-1] < perm[m-i-1] ) ++ tail_rep [ i ];
    }
  }
  return tail_rep;
}

inline uint64_t OrderParameter:: 
_tail_rep_to_index ( std::vector<uint64_t> const& tail_rep ) {
  uint64_t result;
  uint64_t factorial = 1;
  uint64_t m = tail_rep . size ();
  for ( uint64_t i = 1; i < m; ++ i ) {
    factorial *= i;
    result += factorial * (uint64_t) tail_rep [ i ];
  }
  return result;
}

#endif
