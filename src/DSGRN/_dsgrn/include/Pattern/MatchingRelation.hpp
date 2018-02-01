/// MatchinRelation.cpp
/// Shaun Harker
/// MIT LICENSE 
/// 2016-12-26

#pragma once

#ifndef INLINE_IF_HEADER_ONLY
#define INLINE_IF_HEADER_ONLY
#endif

#include "MatchingRelation.h"

INLINE_IF_HEADER_ONLY MatchingRelation::
MatchingRelation ( void ) {}

INLINE_IF_HEADER_ONLY MatchingRelation::
MatchingRelation ( uint64_t dim ) {
  assign(dim);
}

INLINE_IF_HEADER_ONLY void MatchingRelation::
assign ( uint64_t dim ) {
  dimension_ = dim;
}

INLINE_IF_HEADER_ONLY uint64_t MatchingRelation::
dimension ( void ) const {
  return dimension_;
}

INLINE_IF_HEADER_ONLY std::string MatchingRelation::
vertex_labelstring ( uint64_t L ) const {
  std::string result;
  for ( uint64_t d = 0; d < dimension(); ++ d ){
    if ( L & ( 1 << d ) ) { 
      result.push_back('D');
    } else if ( L & ( 1 << (d + dimension() ) ) ) { 
      result.push_back('I');
    } else {
      result.push_back('*');
    }
  }
  return result;
};

INLINE_IF_HEADER_ONLY std::string MatchingRelation::
edge_labelstring ( uint64_t L ) const {
  std::string result;
  uint64_t D = dimension();
  for ( uint64_t d = 0; d < D; ++ d ) {
    int type = ((L & (1 << d)) >> d) | ((L & (1 << (d + D))) >> (d + D - 1));
    if ( type == 0 ) result.push_back('-');
    if ( type == 1 ) result.push_back('M');
    if ( type == 2 ) result.push_back('m');
    if ( type == 3 ) result.push_back('*');
  }
  return result;
};
