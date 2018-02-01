/// OrderParameter.hpp
/// Shaun Harker
/// 2015-05-24

#pragma once

#ifndef INLINE_IF_HEADER_ONLY
#define INLINE_IF_HEADER_ONLY
#endif

#include "OrderParameter.h"

INLINE_IF_HEADER_ONLY OrderParameter::
OrderParameter ( void ) {
  data_ . reset ( new OrderParameter_ );
}

INLINE_IF_HEADER_ONLY OrderParameter::
OrderParameter ( uint64_t m, uint64_t k ) {
  assign ( m, k );
}

INLINE_IF_HEADER_ONLY OrderParameter::
OrderParameter ( std::vector<uint64_t> const& perm ) {
  assign ( perm );
}

INLINE_IF_HEADER_ONLY void OrderParameter::
assign ( uint64_t m, uint64_t k ) {
  data_ . reset ( new OrderParameter_ );
  data_ -> k_ = k;
  data_ -> m_ = m;
  std::vector<uint64_t> tail_rep = _index_to_tail_rep ( k );
  tail_rep . resize ( data_ -> m_ );
  data_ -> permute_ = _tail_rep_to_perm ( tail_rep );
  data_ -> inverse_ . resize ( data_ -> m_ );
  for ( uint64_t i = 0; i < data_ -> m_; ++ i ) data_ -> inverse_[data_ ->permute_[i]]=i;
}

INLINE_IF_HEADER_ONLY void OrderParameter::
assign ( std::vector<uint64_t> const& perm ) {
  data_ . reset ( new OrderParameter_ );
  data_ -> m_ = perm . size ();
  data_ -> permute_ = perm;
  data_ -> k_ = _tail_rep_to_index ( _perm_to_tail_rep ( data_ -> permute_ ) );
  data_ -> inverse_ . resize ( data_ -> m_ );
  for ( uint64_t i = 0; i < data_ -> m_; ++ i ) data_ -> inverse_[data_ ->permute_[i]]=i;
}

INLINE_IF_HEADER_ONLY uint64_t OrderParameter::
operator () ( uint64_t i ) const {
  return data_ -> permute_ [ i ];
}

INLINE_IF_HEADER_ONLY uint64_t OrderParameter::
inverse ( uint64_t i ) const {
  return data_ -> inverse_ [ i ];
}

INLINE_IF_HEADER_ONLY  std::vector<uint64_t> const& OrderParameter::
permutation ( void ) const {
  return data_ -> permute_;
}

INLINE_IF_HEADER_ONLY uint64_t OrderParameter::
index ( void ) const {
  return data_ -> k_;
}

INLINE_IF_HEADER_ONLY uint64_t OrderParameter::
size ( void ) const {
  return data_ -> m_;
}

INLINE_IF_HEADER_ONLY std::string OrderParameter::
stringify ( void ) const {
  std::stringstream ss;
  ss << *this;
  return ss . str ();
}

INLINE_IF_HEADER_ONLY void OrderParameter::
parse ( std::string const& str ) {
  //std::cout << "OrderParameter::parse(" << str << ")\n";
  std::string s = str;
  auto validcharacter = [] (char c) {
    if ( c >= '0' && c <= '9' ) return true;
    return false;
  };
  for ( char & c : s ) if ( not validcharacter ( c ) ) c = ' ';
  std::stringstream ss ( s );
  uint64_t val;
  std::vector<uint64_t> permute;
  while ( ss >> val ) permute . push_back ( val );
  assign ( permute );
}

INLINE_IF_HEADER_ONLY std::vector<OrderParameter> OrderParameter::
adjacencies ( void ) const {
  std::vector<OrderParameter> output;
  // Retrieve the permutation
  std::vector<uint64_t> perm = data_ -> permute_;
  uint64_t N = perm . size ( );
  //
  for ( uint64_t i = 0; i<N-1; ++i ) {
    std::swap( perm[i], perm[i+1] );
    output . push_back ( OrderParameter(perm) );
    std::swap( perm[i], perm[i+1] );
  }
  //
  return output;
}

INLINE_IF_HEADER_ONLY bool OrderParameter::
operator == ( OrderParameter const& rhs ) const {
  if ( data_ -> k_ != rhs . data_ -> k_ ) return false;
  if ( data_ -> m_ != rhs . data_ -> m_ ) return false;
  return true;
}

INLINE_IF_HEADER_ONLY std::ostream& operator << ( std::ostream& stream, OrderParameter const& p ) {
  stream << "[";
  for ( uint64_t i = 0; i <  p.data_ -> m_; ++ i ) {
    if ( i > 0 ) stream << ",";
    stream <<  p.data_ -> permute_[i];
  }
  stream << "]";
  return stream;
}

INLINE_IF_HEADER_ONLY std::vector<uint64_t> OrderParameter::
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

INLINE_IF_HEADER_ONLY std::vector<uint64_t> OrderParameter::
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

INLINE_IF_HEADER_ONLY std::vector<uint64_t> OrderParameter::
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

INLINE_IF_HEADER_ONLY uint64_t OrderParameter::
_tail_rep_to_index ( std::vector<uint64_t> const& tail_rep ) {
  uint64_t result = 0;
  uint64_t factorial = 1;
  uint64_t m = tail_rep . size ();
  for ( uint64_t i = 1; i < m; ++ i ) {
    factorial *= i;
    result += factorial * (uint64_t) tail_rep [ i ];
  }
  return result;
}
