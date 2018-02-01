/// Domain.hpp
/// Shaun Harker
/// 2015-05-24

#pragma once

#ifndef INLINE_IF_HEADER_ONLY
#define INLINE_IF_HEADER_ONLY
#endif

#include "Domain.h"

INLINE_IF_HEADER_ONLY Domain::
Domain ( void ) { 
}

INLINE_IF_HEADER_ONLY Domain::
Domain ( std::vector<uint64_t> const& limits ) {
  assign ( limits );
}

INLINE_IF_HEADER_ONLY void Domain::
assign ( std::vector<uint64_t> const& limits ) {
  limits_ = limits;
  index_ = 0;
  D_ =  limits_ . size ();
  data_ . resize ( D_, 0 );
  offset_ . resize ( D_ );
  max_ = 1;
  for ( uint64_t d = 0; d < D_; ++ d ) {
    offset_[d] =  max_;
    max_ *=  limits_ [ d ];
  }
}

INLINE_IF_HEADER_ONLY uint64_t Domain::
operator [] ( uint64_t d ) const {
  return  data_[d];
}

INLINE_IF_HEADER_ONLY Domain & Domain::
operator ++ ( void ) {
  for ( uint64_t d = 0; d < D_; ++ d ) {
    if ( ++ data_ [ d ] < limits_ [ d ] ) break;
    data_ [ d ] = 0;
  }
  ++  index_;
  return *this;
}

INLINE_IF_HEADER_ONLY Domain Domain::
operator ++ ( int ) {
  Domain result = *this;
  ++ (*this);
  return result;
}

INLINE_IF_HEADER_ONLY uint64_t Domain::
size ( void ) const {
  return  limits_ . size ();
}

INLINE_IF_HEADER_ONLY uint64_t Domain::
index ( void ) const {
  return  index_;
}

INLINE_IF_HEADER_ONLY void Domain::
setIndex ( uint64_t i ) {
  index_ = i;
  for ( uint64_t d = 0; d < D_; ++ d ) {
    data_ [ d ] = i %  limits_ [ d ];
    i /=  limits_ [ d ];
  }
}

INLINE_IF_HEADER_ONLY uint64_t Domain::
left ( uint64_t d ) const {
  return  index_ -  offset_[d];
}

INLINE_IF_HEADER_ONLY uint64_t Domain::
right ( uint64_t d ) const {
  return  index_ +  offset_[d];
}

INLINE_IF_HEADER_ONLY bool Domain::
isMin ( uint64_t d ) const {
  return  data_[d] == 0;
}

INLINE_IF_HEADER_ONLY bool Domain::
isMax ( uint64_t d ) const {
  return  data_[d] ==  limits_[d]-1;
}

INLINE_IF_HEADER_ONLY bool Domain::
isValid ( void ) const {
  return  index_ <  max_;
}

INLINE_IF_HEADER_ONLY std::ostream& 
operator << ( std::ostream& stream, Domain const& dom ) {
  stream << "[";
    bool first = true;
    for ( uint64_t d = 0; d < dom . size (); ++ d ) {
      if ( first ) first = false; else stream << ",";
      stream << "[" << dom[d] << "," << dom[d]+1 << "]";
    }
    stream << "]";
return stream;
}
