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
Components ( std::vector<uint64_t> const& vertices,
             std::vector<bool> const& components,
             std::vector<bool> const& recurrent ) {
  assign ( vertices, components, recurrent );
}

INLINE_IF_HEADER_ONLY void Components::
assign ( std::vector<uint64_t> const& vertices,
         std::vector<bool> const& components,
         std::vector<bool> const& recurrent ) {
  data_ . reset ( new Components_ );

  // for ( uint64_t v : vertices ) {
  //   std::cout << v << " ";
  //   if ( v > vertices . size () ) {
  //     std::cout << "\n";
  //     abort ();
  //   }
  // }
  // std::cout << "\n";

  data_ -> vertices_ = vertices;
  data_ -> components_ = components;
  data_ -> recurrent_ = recurrent;
  data_ -> components_ . resize ( components.size() + 1, true );
  data_ -> which_component_ . resize ( vertices . size () );

  uint64_t N = data_ -> components_ . size ();
  for ( uint64_t i = 0; i < N; ++ i ) {
    if ( data_ -> components_ [ i ] ) {
      data_ -> component_select_ . push_back ( i );
    }
  }

  uint64_t C = recurrent . size ();
  uint64_t R = 0;
  for ( uint64_t i = 0; i < C; ++ i ) {
    if ( data_ -> recurrent_ [ i ] ) {
      data_ -> recurrent_select_ . push_back ( i );
      ++ R;
    }
  }

  //std::cout << "Number of recurrent components = " << recurrent_size << "\n";
  iterator componentBegin ( 0, boost::bind ( &Components_::_component, data_ . get(), _1 ) );
  iterator componentEnd = componentBegin + C;
  iterator recurrentBegin ( 0, boost::bind ( &Components_::_recurrentComponent, data_ . get(), _1 ) );
  iterator recurrentEnd = recurrentBegin + R;
  data_ -> component_container_ = boost::make_iterator_range ( componentBegin, componentEnd );
  data_ -> recurrent_container_ = boost::make_iterator_range ( recurrentBegin, recurrentEnd ); 

  // for ( uint64_t i = 0; i < C; ++ i ) {
  //   auto const& comp = operator [] ( i );
  //   std::cout << "| ";
  //   for ( uint64_t v : comp ) {
  //     std::cout << v << " ";
  //   }
  // }
  // std::cout << "\n";
  for ( uint64_t i = 0; i < C; ++ i ) {
    auto const& comp = operator [] ( i );
    for ( uint64_t v : comp ) {
      data_ -> which_component_ [ v ] = i;
    }
  }
}

INLINE_IF_HEADER_ONLY Components::iterator Components::
begin ( void ) const {
  return data_ -> component_container_ . begin ();
}

INLINE_IF_HEADER_ONLY Components::iterator Components::
end ( void ) const {
  return data_ -> component_container_ . end ();
}

INLINE_IF_HEADER_ONLY uint64_t Components::
size ( void ) const {
  return data_ -> component_container_ . size ();
}

INLINE_IF_HEADER_ONLY Component Components::
operator [] ( uint64_t i ) const {
  return data_ -> component_container_ [ i ]; 
}

INLINE_IF_HEADER_ONLY Components::ComponentContainer const& Components::
recurrentComponents ( void ) const { return data_ -> recurrent_container_; } 

INLINE_IF_HEADER_ONLY bool Components::
isRecurrent ( int64_t i ) const { return data_ -> recurrent_ [ i ]; }

INLINE_IF_HEADER_ONLY uint64_t Components::
whichComponent ( uint64_t i ) const {
  return data_ -> which_component_ [ i ];
}

INLINE_IF_HEADER_ONLY std::ostream& 
operator << ( std::ostream& stream, Components const& c ) {
  stream << "[";
  bool outer_first = true;
  for ( auto const& component : c ) {
    if ( outer_first ) outer_first = false; else stream << ",";
    stream << "[";
    bool first = true;
    for ( auto v : component ) {
      if ( first ) first = false; else stream << ",";
      stream << v;
    }
    stream << "]";
  }
  stream << "]";
  return stream;
}

  /// component
  ///  Return the ith component
INLINE_IF_HEADER_ONLY Component Components_:: 
_component ( int64_t i ) {
  return Component ( vertices_ . begin () + component_select_ [ i ], 
                     vertices_ . begin () + component_select_ [ i + 1 ] );
}

/// recurrentComponent
///    Return the "rank"th recurrent component
INLINE_IF_HEADER_ONLY Component Components_:: 
_recurrentComponent ( int64_t rank ) {
  return _component ( recurrent_select_ [ rank ] );
}
#endif
