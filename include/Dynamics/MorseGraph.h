/// MorseGraph.h
/// Shaun Harker
/// 2015-05-24

#ifndef DSGRN_MORSEGRAPH_H
#define DSGRN_MORSEGRAPH_H

#include "Dynamics/Annotation.h"
#include "Dynamics/MorseDecomposition.h"
#include "Graph/Poset.h"
#include "Database/SHA256.h"
#include "Database/json.h"

#include <memory>
#include <cstdlib>
#include <string>
#include <sstream>
#include <unordered_map>

#include "boost/serialization/serialization.hpp"
#include "boost/serialization/shared_ptr.hpp"
#include "boost/serialization/unordered_map.hpp"

/// class MorseGraph_
class MorseGraph_  {
public:
  /// assign (Morse Decomposition)
  ///   Assign data to Morse Graph
  template < class SwitchingGraph > void
  assign ( SwitchingGraph const& sg,
           MorseDecomposition const& md );

  /// poset
  ///   Access poset
  Poset const
  poset ( void ) const;

  /// annotation
  ///   Return the annotation on vertex v.
  Annotation const
  annotation ( uint64_t v ) const;

  /// SHA
  ///   Return a SHA-256 code
  std::string
  SHA256 ( void ) const;

  /// stringify
  ///   Return a JSON description
  std::string
  stringify ( void ) const;

  /// parse
  ///   Initialize from a JSON description
  void
  parse ( std::string const& str );

  /// operator <<
  ///   Stream information to graphviz format
  friend std::ostream& operator << ( std::ostream& stream, MorseGraph_ const& md );

private:
  Poset poset_;
  std::unordered_map<uint64_t, Annotation> annotations_;

  /// _canonicalize
  ///   Renumber the vertices in a "canonical" fashion
  ///   given the available information. (Note: not every
  ///   isomorphic graph has the same canonicalization.)
  void _canonicalize ( void );

  /// serialize
  ///   For use with BOOST Serialization library,
  ///   which is used by the cluster-delegator MPI package
  friend class boost::serialization::access;
  template<class Archive>
  void serialize(Archive & ar, const unsigned int version) {
    ar & poset_;
    ar & annotations_;
  }
};

template < class SwitchingGraph > void MorseGraph_::
assign ( SwitchingGraph const& sg,
         MorseDecomposition const& md ) {
  // Copy the poset
  poset_ = md . poset ();
  // Compute the annotations
  uint64_t N = poset_ . size ();
  for ( uint64_t v = 0; v < N; ++ v ) {
    annotations_[v] = sg . annotate ( * md . components () [ v ] );
  }
  // Canonicalize the graph
  _canonicalize ();
}

inline Poset const MorseGraph_::
poset ( void ) const {
  return poset_;
}

inline Annotation const MorseGraph_::
annotation ( uint64_t v ) const {
  return annotations_ . find ( v ) -> second;
}

inline std::string MorseGraph_::
stringify ( void ) const {
  std::stringstream ss;
  ss << "{\"poset\":";
  ss << poset_ . stringify ();
  ss << ",\"annotations\":[";
  uint64_t N = poset_ . size ();
  bool first = true;
  for ( uint64_t v = 0; v < N; ++ v ) {
    if ( first ) first = false; else ss << ",";
    ss << annotation ( v ) . stringify ();
  }
  ss << "]}";
  return ss . str ();
}

inline void MorseGraph_::
parse ( std::string const& str ) {
  std::shared_ptr<JSON::Object> json = JSON::toObject(JSON::parse(str));
  poset_ . parse ( JSON::stringify ( (*json)[std::string("poset")] ));
  annotations_ . clear ();
  std::shared_ptr<JSON::Array> annotation_array = 
    JSON::toArray((*json)[std::string("annotations")]);
  uint64_t N = annotation_array -> size ();
  for ( uint64_t v = 0; v < N; ++ v ) {
    annotations_ [ v ] . parse ( JSON::stringify ( (*annotation_array)[v] ));
  } 
}


inline std::ostream& operator << ( std::ostream& stream, MorseGraph_ const& md ) {
  Poset const poset = md . poset ();
  stream << "digraph {\n";
  for ( uint64_t v = 0; v < poset . size (); ++ v ) {
    stream << v;
    stream << "[label=\"";
    bool first_item = true;
    for ( auto s : md . annotation ( v ) ) {
      if ( first_item ) first_item = false; else stream << ", ";
      stream << s;
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

inline std::string MorseGraph_::
SHA256 ( void ) const {
  std::stringstream ss;
  ss << *this;
  return sha256 ( ss . str () );
}
  
inline void MorseGraph_::
_canonicalize ( void ) {
}

#endif
