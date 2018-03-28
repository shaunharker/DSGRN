/// Poset.hpp
/// Shaun Harker
/// 2015-05-15

#pragma once

#ifndef INLINE_IF_HEADER_ONLY
#define INLINE_IF_HEADER_ONLY
#endif

#include "Poset.h"

INLINE_IF_HEADER_ONLY Poset::
Poset ( void ) {
  data_ . reset ( new Poset_ );
}

INLINE_IF_HEADER_ONLY Poset::
Poset ( std::vector<std::vector<uint64_t>> & adjacencies ) {
  assign ( adjacencies );
}

INLINE_IF_HEADER_ONLY Poset::
Poset ( Digraph const& digraph ) {
  assign ( digraph );
}

INLINE_IF_HEADER_ONLY void Poset::
assign ( std::vector<std::vector<uint64_t>> & adjacencies ) {
  assign ( Digraph ( adjacencies ) );
}

INLINE_IF_HEADER_ONLY void Poset::
assign ( Digraph const& digraph ) {
  data_ . reset ( new Poset_ );
  data_ -> transitive_closure = digraph . transitive_closure ();
  data_ -> transitive_reduction = data_ -> transitive_closure . transitive_reduction ();
  data_ -> transpose_transitive_closure = data_ -> transitive_closure . transpose ();
  data_ -> transpose_transitive_reduction = data_ -> transitive_reduction . transpose ();
  uint64_t N = size ();
  data_ -> relation . resize ( N );
  for ( uint64_t i = 0; i < N; ++ i ) {
    for ( uint64_t j : descendants ( i ) ) {
      data_ -> relation[i] . insert(j);
    }
  }
}

INLINE_IF_HEADER_ONLY uint64_t Poset::
size ( void ) const {
  return data_ -> transitive_reduction . size ();
}

INLINE_IF_HEADER_ONLY std::vector<uint64_t> const& Poset::
parents ( uint64_t v ) const {
  return data_ -> transpose_transitive_reduction . adjacencies ( v );
}

INLINE_IF_HEADER_ONLY std::vector<uint64_t> const& Poset::
children ( uint64_t v ) const {
  return data_ -> transitive_reduction . adjacencies ( v );
}

INLINE_IF_HEADER_ONLY std::vector<uint64_t> const& Poset::
ancestors ( uint64_t v ) const {
  return data_ -> transpose_transitive_closure . adjacencies ( v );
}

INLINE_IF_HEADER_ONLY std::vector<uint64_t> const& Poset::
descendants ( uint64_t v ) const {
  return data_ -> transitive_closure . adjacencies ( v );
}

INLINE_IF_HEADER_ONLY std::set<uint64_t> Poset::
maximal ( std::set<uint64_t> const& elements ) const {
  std::set<uint64_t> result;
  auto is_maximal = [&](uint64_t u){
    for ( uint64_t v : elements ) if ( compare (u, v) ) return false;
    return true;
  };
  for ( uint64_t u : elements ) if ( is_maximal(u) ) result . insert ( u );
  return result;
}

INLINE_IF_HEADER_ONLY bool Poset::
compare ( const uint64_t & u, const uint64_t & v ) const {
  return data_ -> relation[u] . count (v);
}

INLINE_IF_HEADER_ONLY Poset Poset::
permute ( const std::vector<uint64_t> & permutation ) const {
  Poset result;
  result . data_ -> transitive_closure = data_ -> transitive_closure . permute ( permutation );
  result . data_ -> transitive_reduction = data_ -> transitive_reduction . permute ( permutation );
  result . data_ -> transpose_transitive_closure = data_ -> transpose_transitive_closure . permute ( permutation );
  result . data_ -> transpose_transitive_reduction = data_ -> transpose_transitive_reduction . permute ( permutation );
  uint64_t N = size ();
  result . data_ -> relation . resize ( N );
  for ( uint64_t i = 0; i < N; ++ i ) {
    for ( uint64_t j : result . data_ -> transitive_closure . adjacencies ( i ) ) {
      result . data_ -> relation[i] . insert ( j );
    }
  }
  return result;  
}

INLINE_IF_HEADER_ONLY std::string Poset::
stringify ( void ) const {
  return data_ -> transitive_reduction . stringify ();
}

INLINE_IF_HEADER_ONLY Poset & Poset::
parse ( std::string const& str ) {
  Digraph digraph;
  digraph . parse ( str );
  assign ( digraph );
  return *this;
}

INLINE_IF_HEADER_ONLY std::string Poset::
graphviz ( void ) const {
  return data_ -> transitive_reduction . graphviz ();
}

INLINE_IF_HEADER_ONLY std::ostream& operator << ( std::ostream& stream, Poset const& poset ) {
  return stream << poset . data_ -> transitive_reduction;
}
