/// Annotation.hpp
/// Shaun Harker
/// 2015-05-15

#pragma once

#ifndef INLINE_IF_HEADER_ONLY
#define INLINE_IF_HEADER_ONLY
#endif

#include "Annotation.h"

INLINE_IF_HEADER_ONLY Annotation::
Annotation ( void ) { 
  data_ . reset ( new Annotation_ );
}

INLINE_IF_HEADER_ONLY uint64_t Annotation::
size ( void ) const {
  return data_ ->annotations_ . size ();
}

INLINE_IF_HEADER_ONLY Annotation::iterator Annotation::
begin ( void ) const {
  return data_ ->annotations_ . begin ();
}

INLINE_IF_HEADER_ONLY Annotation::iterator Annotation::
end ( void ) const {
  return data_ ->annotations_ . end ();
}

INLINE_IF_HEADER_ONLY std::string const& Annotation::
operator [] ( uint64_t i ) const {
  return data_ ->annotations_[i];
}

INLINE_IF_HEADER_ONLY void Annotation::
append ( std::string const& label ) {
  data_ -> annotations_ . push_back ( label );
}

INLINE_IF_HEADER_ONLY std::string Annotation::
stringify ( void ) const {
  std::stringstream ss;
  ss << "[";
    bool first = true;
    for ( std::string const& s : data_ ->annotations_ ) {
      if ( first ) first = false; else ss << ",";
      ss << "\"" << s << "\"";
    }
    ss << "]";
return ss . str ();
}

INLINE_IF_HEADER_ONLY Annotation & Annotation::
parse ( std::string const& str ) {
  json array = json::parse(str);
  data_ -> annotations_ . clear ();
  for ( std::string const& s : array ) {
    data_ -> annotations_ . push_back ( s );
  } 
  return *this;
}

INLINE_IF_HEADER_ONLY std::ostream& operator << ( std::ostream& stream, Annotation const& a ) {
  stream << "{";
  bool first = true;
  for ( auto x : a . data_ ->annotations_ ) {
    if ( first ) first = false; else stream << ", ";
    stream << "\"" << x << "\"";
  }
  stream << "}";
  return stream;
}
