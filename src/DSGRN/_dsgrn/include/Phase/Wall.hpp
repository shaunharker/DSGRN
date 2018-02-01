/// Wall.hpp
/// Shaun Harker
/// 2015-05-27

#pragma once

#ifndef INLINE_IF_HEADER_ONLY
#define INLINE_IF_HEADER_ONLY
#endif

#include "Wall.h"

INLINE_IF_HEADER_ONLY Wall::
Wall ( void ) {
  //data_ . reset ( new Wall_ );
}

INLINE_IF_HEADER_ONLY Wall::
Wall ( Domain const& dom, uint64_t collapse_dim, int direction ) {
  assign ( dom, collapse_dim, direction );
}

INLINE_IF_HEADER_ONLY void Wall::
assign ( Domain const& dom, uint64_t collapse_dim, int direction ) {
  //data_ . reset ( new Wall_ );
  uint64_t dom_index = (direction == 1) ? dom . right (collapse_dim) : dom . index ();
  index_ = (dom_index << dom . size ()) | (1LL << collapse_dim);
}
  
INLINE_IF_HEADER_ONLY uint64_t Wall::
index ( void ) const {
  return  index_;
}

INLINE_IF_HEADER_ONLY std::ostream& 
operator << ( std::ostream& stream, Wall const& w ) {
  stream << w . index ();
  return stream;
}
