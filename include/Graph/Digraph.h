/// Digraph.h
/// Shaun Harker
/// 2015-05-24

#ifndef DSGRN_DIGRAPH_H
#define DSGRN_DIGRAPH_H

#include "common.h"

struct Digraph_;

/// class Digraph
///   This class handles storage of edges between 
///   vertices in the form of adjacency lists.
class Digraph {
public:
  /// constructor
  Digraph ( void );

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
  friend std::ostream& operator << ( std::ostream& stream, Digraph const& dg );

protected:
  std::shared_ptr<Digraph_> data_;
  /// serialize
  ///   For use with BOOST Serialization library,
  ///   which is used by the cluster-delegator MPI package
  friend class boost::serialization::access;
  template<class Archive>
  void serialize(Archive & ar, const unsigned int version) {
    ar & data_;
  }
  
};

struct Digraph_ {
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

#endif
