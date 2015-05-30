/// Digraph.h
/// Shaun Harker
/// 2015-05-24

#ifndef DSGRN_DIGRAPH_H
#define DSGRN_DIGRAPH_H

#include <vector>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include "Database/json.h"

#include "boost/serialization/serialization.hpp"
#include "boost/serialization/vector.hpp"

/// class Digraph_
///   This class handles storage of edges between 
///   vertices in the form of adjacency lists.
class Digraph_ {
public:
  /// adjacencies (getter)
  ///   Return vector of Vertices which are out-edge adjacencies of input v
  std::vector<uint64_t> const&
  adjacencies ( uint64_t v ) const;

  /// adjacencies (setter)
  ///   Return vector of Vertices which are out-edge adjacencies of input v
  std::vector<uint64_t> &
  adjacencies ( uint64_t v );

  /// size
  ///   Return number of vertices
  uint64_t 
  size ( void ) const;

  /// resize(n)
  ///   Resize so there n vertices.
  ///   Creates empty adjacency lists if needed.
  ///   n must not be smaller than current size
  void
  resize ( uint64_t n );

  /// add_vertex
  ///   Add a vertex, and return the 
  ///   index of the newly added vertex.
  uint64_t
  add_vertex ( void );

  /// add_edge
  ///   Add an edge between source and target
  ///   Warning: does not check if edge already exists
  void
  add_edge ( uint64_t source, uint64_t target );

  /// stringify
  ///   Return a JSON description
  std::string
  stringify ( void ) const;

  /// parse
  ///   Initialize from a JSON description
  void
  parse ( std::string const& str );

  /// operator <<
  ///   Emit data to stream in graphviz format
  friend std::ostream& operator << ( std::ostream& stream, Digraph_ const& dg );

protected:
  std::vector<std::vector<uint64_t>> adjacencies_;
  /// serialize
  ///   For use with BOOST Serialization library,
  ///   which is used by the cluster-delegator MPI package
  friend class boost::serialization::access;
  template<class Archive>
  void serialize(Archive & ar, const unsigned int version) {
    ar & adjacencies_;
  }
  
};

inline std::vector<uint64_t> const& Digraph_::
adjacencies ( uint64_t v ) const {
  return adjacencies_ [ v ];
}

inline std::vector<uint64_t> & Digraph_::
adjacencies ( uint64_t v ) {
  return adjacencies_ [ v ];
}

inline uint64_t Digraph_::
size ( void ) const {
  return adjacencies_.size();
}

inline void Digraph_::
resize ( uint64_t n ) {
  adjacencies_ . resize ( n );
}
  
inline uint64_t Digraph_::
add_vertex ( void ) {
  adjacencies_ . push_back ( std::vector<uint64_t> () );
  return adjacencies_ . size () - 1;
}

inline void Digraph_::
add_edge ( uint64_t source, uint64_t target ) {
  adjacencies_[source].push_back(target);
}

inline std::string Digraph_::
stringify ( void ) const {
  std::stringstream ss;
  ss << "[";
  bool outerfirst = true;
  for ( auto const& adjlist : adjacencies_ ) {
    if ( outerfirst ) outerfirst = false; else ss << ",";
    ss << "[";
    bool innerfirst = true;
    for ( uint64_t v : adjlist ) {
      if ( innerfirst ) innerfirst = false; else ss << ",";
      ss << v;
    }
    ss << "]";
  }
  ss << "]";
  return ss . str ();
}

inline void Digraph_::
parse ( std::string const& str ) {
  std::shared_ptr<JSON::Array> array = JSON::toArray(JSON::parse(str));
  uint64_t N = array -> size ();
  adjacencies_ . clear ();
  adjacencies_ . resize ( N );
  for ( uint64_t source = 0; source < N; ++ source ) {
    std::shared_ptr<JSON::Array> adjlist = JSON::toArray((*array)[source]);
    for ( auto target : *adjlist ) {
      adjacencies_[source] . push_back ( *JSON::toInteger(target) );
    }
  }
}

inline std::ostream& operator << ( std::ostream& stream, Digraph_ const& dg ) {
  stream << "digraph {\n";
  for ( uint64_t v = 0; v < dg . size (); ++ v ) {
    stream << v << ";\n";
  }
  for ( uint64_t source = 0; source < dg . size (); ++ source ) {
    for ( uint64_t target : dg . adjacencies ( source ) ) {
      stream << source << " -> " << target << ";\n";
    }
  }
  stream << "}\n";
  return stream;
}


#endif
