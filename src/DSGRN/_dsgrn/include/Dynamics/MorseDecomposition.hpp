/// MorseDecomposition.hpp
/// Shaun Harker
/// 2015-05-24

#pragma once

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
  data_ -> components_ = components;

  uint64_t C = components . size ();
  uint64_t R = components . recurrentComponents () . size ();
  std::vector<std::vector<uint64_t>> reachability ( R );
  std::vector<uint64_t> recurrent_indices;
  for ( uint64_t i = 0; i < C; ++ i ) {
    if ( components . isRecurrent ( i ) ) {
      recurrent_indices . push_back ( i );
    }
  }
  // Proceed in cohorts of 64 recurrent components. There are ceil(R/64) cohorts
  uint64_t num_cohorts = R / 64;
  if ( R % 64 != 0 ) ++ num_cohorts; // Round up for ceiling
  std::vector<uint64_t> reach_info ( C, 0 );
  for ( uint64_t cohort = 0; cohort < num_cohorts; ++ cohort ) {
    if ( cohort > 0 ) std::fill ( reach_info.begin(), reach_info.end(), 0);
    uint64_t source = 64LL*cohort;
    // Give each recurrent component in the cohort a unique bit signature
    // e.g. the 5th component gets 000...00010000
    for ( uint64_t i = 0; i < 64; ++ i, ++ source ) {
      if ( source == R ) break;
      reach_info [ recurrent_indices [ source ] ] = (1LL << i);
    }
    // Propagate reachability information by bitwise-oring bit signatures
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
        if ( code & 1 ) reachability[ancestor].push_back(i);
        code >>= 1;
        ++ ancestor;
      }
    }
  } 
  data_ -> poset_ = Poset(reachability);
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

INLINE_IF_HEADER_ONLY std::vector<uint64_t> MorseDecomposition::
morseset ( uint64_t i ) const {
  auto component = recurrent()[i];
  return std::vector<uint64_t>(component.begin(), component.end());
}

INLINE_IF_HEADER_ONLY std::string MorseDecomposition::
graphviz ( void ) const {
  std::stringstream ss;
  ss << *this;
  return ss . str ();
}

INLINE_IF_HEADER_ONLY std::ostream& operator << ( std::ostream& stream, MorseDecomposition const& md ) {
  Poset const poset = md . poset ();
  stream << "digraph {\n";
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
    for ( uint64_t target : poset . children ( source ) ) {
      stream << source << " -> " << target << ";\n";
    }
  }
  stream << "}\n";
  return stream;
}

INLINE_IF_HEADER_ONLY void MorseDecomposition::
_canonicalize ( void ) {
}
