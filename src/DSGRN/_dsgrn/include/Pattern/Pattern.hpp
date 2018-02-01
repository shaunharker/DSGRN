/// Pattern.hpp
/// MIT LICENSE
/// Shaun Harker and Bree Cummins
/// 2016-03-21

#pragma once

#ifndef INLINE_IF_HEADER_ONLY
#define INLINE_IF_HEADER_ONLY
#endif

#include "Pattern.h"

INLINE_IF_HEADER_ONLY Pattern::
Pattern ( void ) {
  data_ . reset ( new Pattern_ );
}

INLINE_IF_HEADER_ONLY Pattern::
Pattern ( Poset const& poset, 
          std::vector<uint64_t> const& events,
          uint64_t final_label,
          uint64_t dimension ) {
  assign ( poset, events, final_label, dimension );
}

INLINE_IF_HEADER_ONLY void Pattern::
assign ( Poset const& poset, 
         std::vector<uint64_t> const& events, 
         uint64_t final_label,
         uint64_t dimension ) {
  data_ . reset ( new Pattern_ );
  data_ -> poset_ = poset;
  data_ -> events_ = events;
  data_ -> label_ = final_label;
  data_ -> dimension_ = dimension;
}

INLINE_IF_HEADER_ONLY void Pattern::
load ( std::string const& filename ) {
  std::ifstream infile ( filename );
  if ( not infile . good () ) { 
    throw std::runtime_error ( "Problem loading pattern specification file " + filename );
  }
  std::stringstream buffer;
  buffer << infile.rdbuf();
  parse ( buffer . str () ); 
}

INLINE_IF_HEADER_ONLY uint64_t Pattern::
label ( void ) const {
  return data_ -> label_;
}

INLINE_IF_HEADER_ONLY uint64_t Pattern::
dimension ( void ) const {
  return data_ -> dimension_;
}

INLINE_IF_HEADER_ONLY Poset Pattern::
poset ( void ) const {
  return data_ -> poset_;
}

INLINE_IF_HEADER_ONLY uint64_t Pattern::
event ( uint64_t v ) const {
  return data_ -> events_ [ v ];
}

INLINE_IF_HEADER_ONLY std::string Pattern::
stringify ( void ) const {
  std::stringstream ss;
  ss << "{";
  ss << "\"poset\":";
  ss << data_ -> poset_ . stringify ();
  ss << ",\"events\":";
  ss << json(data_ -> events_); 
  ss << ",\"label\":";
  ss << data_ -> label_;
  ss << ",\"dimension\":";
  ss << data_ -> dimension_;
  ss << "}";
  return ss . str ();
}

INLINE_IF_HEADER_ONLY void Pattern::
parse ( std::string const& str ) {
  data_ . reset ( new Pattern_ );
  json j = json::parse(str);
  data_ -> poset_ . parse ( j["poset"].dump() );
  for ( auto event : j["events"] ) data_ -> events_ . push_back ( event );
  data_ -> label_ = j["label"];
  data_ -> dimension_ = j["dimension"];
}
