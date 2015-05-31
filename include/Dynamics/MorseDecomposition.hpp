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
  data_ -> recurrent_ = Components ();
  std::vector<std::shared_ptr<std::vector<uint64_t>>> recurrent_components;
  std::vector<uint64_t> recurrent_indices;
  uint64_t u_comp = 0;
  uint64_t recurrent_count = 0;
  std::unordered_map<uint64_t, std::unordered_set<uint64_t>> reach_info;
  for ( auto const& component : components ) {
    bool is_recurrent = false;
    if ( component -> size () > 1 ) { 
      is_recurrent = true;
    } else {
      uint64_t u = (*component)[0];
      std::vector<uint64_t> const& children = digraph . adjacencies ( u );
      for ( uint64_t v : children ) {
        if ( u == v ) {
          is_recurrent = true;
          break;
        }
      }
    }
    if ( is_recurrent ) {
      // Recurrent component
      recurrent_components . push_back ( component );
      recurrent_indices . push_back ( u_comp );
      reach_info [ u_comp ] . insert ( recurrent_count );
      data_ -> poset_ . add_vertex ();
      for ( uint64_t ancestor : reach_info [ u_comp ] ) {
        data_ ->poset_ . add_edge ( ancestor, recurrent_count );
      }
      ++ recurrent_count;
    }
    // Loop through all vertices in the components
    for ( uint64_t u : *component ) {
      // Find the children
      std::vector<uint64_t> const& children =
        digraph . adjacencies ( u );
      // Find the components the children live in
      std::unordered_set<uint64_t> target_components;
      for ( uint64_t v : children ) {
        uint64_t v_comp = components . whichComponent ( v );
        target_components . insert ( v_comp );
      }
      // Propagate reachability data into the target components
      for ( uint64_t v_comp : target_components ) {
        for ( uint64_t recurrent : reach_info [ u_comp ] ) {
          reach_info [ v_comp ] . insert ( recurrent ); 
        }
      }
    }
    reach_info . erase ( u_comp );
    ++ u_comp;
  }   
  data_ ->recurrent_ . assign ( recurrent_components );
  data_ ->poset_ . reduction ();
  _canonicalize ();
}

INLINE_IF_HEADER_ONLY Poset const MorseDecomposition::
poset ( void ) const {
  return data_ ->poset_;
}

INLINE_IF_HEADER_ONLY Components const MorseDecomposition::
components ( void ) const {
  return data_ ->recurrent_;
}

INLINE_IF_HEADER_ONLY std::ostream& operator << ( std::ostream& stream, MorseDecomposition const& md ) {
  Poset const poset = md . poset ();
  stream << "digraph g {\n";
  for ( uint64_t v = 0; v < poset . size (); ++ v ) {
    stream << v;
    stream << "[label=\"";
    bool first_item = true;
    for ( uint64_t u : * md . data_ ->recurrent_ [ v ] ) {
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
