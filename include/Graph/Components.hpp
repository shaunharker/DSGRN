/// Components.hpp
/// Shaun Harker
/// 2015-05-24

#ifndef DSGRN_COMPONENTS_HPP
#define DSGRN_COMPONENTS_HPP

#ifndef INLINE_IF_HEADER_ONLY
#define INLINE_IF_HEADER_ONLY
#endif

#include "Components.h"

INLINE_IF_HEADER_ONLY Components::
Components ( void ) { 
  data_ . reset ( new Components_ );
}

INLINE_IF_HEADER_ONLY Components::
Components ( std::vector<std::shared_ptr<std::vector<uint64_t>>> const& SCCs ) {
  assign ( SCCs );
}

INLINE_IF_HEADER_ONLY void Components::
assign ( std::vector<std::shared_ptr<std::vector<uint64_t>>> const& SCCs ) {
  data_ . reset ( new Components_ );
  data_ -> which_component_ . clear ();
  data_ -> components_ = SCCs;
  for ( uint64_t comp_num = 0; comp_num < data_ ->components_ . size (); ++ comp_num ) {
    for ( uint64_t v : * data_ ->components_ [ comp_num ] ) {
      data_ ->which_component_ [ v ] = comp_num;
    }
  }
}

INLINE_IF_HEADER_ONLY Components::iterator Components::
begin ( void ) const {
  return data_ ->components_ . begin ();
}

INLINE_IF_HEADER_ONLY Components::iterator Components::
end ( void ) const {
  return data_ ->components_ . end ();
}

INLINE_IF_HEADER_ONLY uint64_t Components::
size ( void ) const {
  return data_ ->components_ . size ();
}

INLINE_IF_HEADER_ONLY std::shared_ptr<std::vector<uint64_t>> Components::
operator [] ( uint64_t i ) const {
  return data_ ->components_ [ i ];
}

INLINE_IF_HEADER_ONLY uint64_t Components::
whichComponent ( uint64_t i ) const {
  auto it = data_ ->which_component_ . find ( i );
  if ( it == data_ ->which_component_  . end () ) return data_ ->components_ . size ();
  return it -> second;
}

INLINE_IF_HEADER_ONLY std::ostream& 
operator << ( std::ostream& stream, Components const& c ) {
  stream << "[";
  bool outer_first = true;
  for ( auto const& component : c ) {
    if ( outer_first ) outer_first = false; else stream << ".";
    stream << "[";
    bool first = true;
    for ( auto v : *component ) {
      if ( first ) first = false; else stream << ",";
      stream << v;
    }
    stream << "]";
  }
  stream << "]";
  return stream;
}


#endif
