/// MorseDecomposition.h
/// Shaun Harker
/// 2015-05-24

#ifndef DSGRN_MORSEDECOMPOSITION_H
#define DSGRN_MORSEDECOMPOSITION_H

#include <iostream>
#include <memory>
#include <vector>
#include <unordered_set>

#include "Graph/Digraph.h"
#include "Graph/Components.h"
#include "Graph/Poset.h"
#include "Graph/StrongComponents.h"

class MorseDecomposition_ {
public:
  /// assign
  ///   Create a Morse Decomposition given
  ///   a digraph.
  void
  assign ( Digraph const& digraph );

  /// assign
  ///   Create a Morse Decomposition given
  ///   a digraph and its strong components
  ///   (This method is provided in case 
  ///    strong components already computed.)
  void
  assign ( Digraph const& digraph, 
           Components const& components );

  /// poset 
  ///   Return the underlying partial order
  Poset const
  poset ( void ) const;

  /// components 
  ///   Return recurrent components 
  Components const
  components ( void ) const;

  /// operator <<
  ///   Stream information to graphviz format
  friend std::ostream& operator << ( std::ostream& stream, MorseDecomposition_ const& md );

private:
  Digraph digraph_;
  Components recurrent_;
  Poset poset_;
  /// _canonicalize
  ///   Renumber the vertices in a "canonical" fashion
  ///   given the available information. (Note: not every
  ///   isomorphic graph has the same canonicalization.)
  void _canonicalize ( void );
};

inline void MorseDecomposition_::
assign ( Digraph const& digraph ) {
  Components components = StrongComponents ( digraph );
  assign ( digraph, components );
}

inline void MorseDecomposition_::
assign ( Digraph const& digraph, 
         Components const& components ) {
  poset_ = Poset ();
  recurrent_ = Components ();
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
      poset_ . add_vertex ();
      for ( uint64_t ancestor : reach_info [ u_comp ] ) {
        poset_ . add_edge ( ancestor, recurrent_count );
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
  recurrent_ . assign ( recurrent_components );
  poset_ . reduction ();
  _canonicalize ();
}

inline Poset const MorseDecomposition_::
poset ( void ) const {
  return poset_;
}

inline Components const MorseDecomposition_::
components ( void ) const {
  return recurrent_;
}

inline std::ostream& operator << ( std::ostream& stream, MorseDecomposition_ const& md ) {
  Poset const poset = md . poset ();
  stream << "digraph g {\n";
  for ( uint64_t v = 0; v < poset . size (); ++ v ) {
    stream << v;
    stream << "[label=\"";
    bool first_item = true;
    for ( uint64_t u : * md . recurrent_ [ v ] ) {
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

inline void MorseDecomposition_::
_canonicalize ( void ) {
}

#endif
