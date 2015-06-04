/// MorseDecomposition.hpp
/// Shaun Harker
/// 2015-05-24

#ifndef DSGRN_MORSEDECOMPOSITION_HPP
#define DSGRN_MORSEDECOMPOSITION_HPP

#ifndef INLINE_IF_HEADER_ONLY
#define INLINE_IF_HEADER_ONLY
#endif

#include "MorseDecomposition.h"

INLINE_IF_HEADER_ONLY MorseDecomposition::
MorseDecomposition ( void ) { 
  data_ . reset ( new MorseDecomposition_ );
}

INLINE_IF_HEADER_ONLY MorseDecomposition::
MorseDecomposition ( Digraph const& digraph ) {
  assign ( digraph );
}

INLINE_IF_HEADER_ONLY MorseDecomposition::
MorseDecomposition ( Digraph const& digraph, 
                     Components const& components ) {
  assign ( digraph, components );
}

INLINE_IF_HEADER_ONLY void MorseDecomposition::
assign ( Digraph const& digraph ) {
  //data_ . reset ( new MorseDecomposition_ );
  Components components = StrongComponents ( digraph );
  assign ( digraph, components );
}

INLINE_IF_HEADER_ONLY void MorseDecomposition::
assign ( Digraph const& digraph, 
         Components const& components ) {
  data_ . reset ( new MorseDecomposition_ );
  data_ -> poset_ = Poset ();
  data_ -> components_ = components;

  uint64_t C = components . size ();
  uint64_t R = components . recurrentComponents () . size ();
  std::vector<uint64_t> recurrent_indices;
  for ( uint64_t i = 0; i < C; ++ i ) {
    if ( components . isRecurrent ( i ) ) {
      recurrent_indices . push_back ( i );
      data_ -> poset_ . add_vertex ();
    }
  }
  // Proceed in cohorts of 64
  uint64_t num_cohorts = R / 64;
  if ( R % 64 != 0 ) ++ num_cohorts;
  std::vector<uint64_t> reach_info ( C, 0 );
  for ( uint64_t cohort = 0; cohort < num_cohorts; ++ cohort ) {
    if ( cohort > 0 ) std::fill ( reach_info.begin(), reach_info.end(), 0);
    uint64_t source = 64LL*cohort;
    for ( uint64_t i = 0; i < 64; ++ i, ++ source ) {
      if ( source == R ) break;
      reach_info [ recurrent_indices [ source ] ] = (1LL << i);
    }
    uint64_t parent_comp = 0;
    for ( auto const& component : components ) { 
      for ( uint64_t u : component ) {
        std::vector<uint64_t> const& children = digraph . adjacencies ( u );
        for ( uint64_t v : children ) {
          uint64_t child_comp = components . whichComponent ( v );
          reach_info [ child_comp ] |= reach_info [ parent_comp ];
        }
      }
      ++ parent_comp;
    }
    for ( uint64_t i = 0; i < R; ++ i ) {
      uint64_t code = reach_info [ recurrent_indices [ i ] ];
      uint64_t ancestor = 64*cohort;
      while ( code != 0 ) {
        if ( code & 1 ) data_ -> poset_ . add_edge ( ancestor, i );
        code >>= 1;
        ++ ancestor;
      }
    }
  } 
  data_ -> poset_ . reduction ();
  _canonicalize ();
}

INLINE_IF_HEADER_ONLY Poset const MorseDecomposition::
poset ( void ) const {
  return data_ ->poset_;
}

INLINE_IF_HEADER_ONLY Components const MorseDecomposition::
components ( void ) const {
  return data_ -> components_;
}

INLINE_IF_HEADER_ONLY Components::ComponentContainer const MorseDecomposition::
recurrent ( void ) const {
  return data_ -> components_ . recurrentComponents ();
}

INLINE_IF_HEADER_ONLY std::ostream& operator << ( std::ostream& stream, MorseDecomposition const& md ) {
  Poset const poset = md . poset ();
  stream << "digraph g {\n";
  for ( uint64_t v = 0; v < poset . size (); ++ v ) {
    stream << v;
    stream << "[label=\"";
    bool first_item = true;
    for ( uint64_t u : md . recurrent () [ v ] ) {
      if ( first_item ) first_item = false; else stream << ", ";
      stream << u;
    }
    stream << "\"];\n";
  }
  for ( uint64_t source = 0; source < poset . size (); ++ source ) {
    for ( uint64_t target : poset . adjacencies ( source ) ) {
      stream << source << " -> " << target << ";\n";
    }
  }
  stream << "}\n";
  return stream;
}

INLINE_IF_HEADER_ONLY void MorseDecomposition::
_canonicalize ( void ) {
}

#endif
