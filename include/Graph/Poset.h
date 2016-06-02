/// Poset.h
/// Shaun Harker
/// 2015-05-15

#ifndef DSGRN_POSET_H
#define DSGRN_POSET_H

#include "common.h"

#include "Graph/Digraph.h"

struct Poset_;

/// class Poset
class Poset {
public:
  /// Poset
  ///   Default constructor
  Poset ( void );

  /// Poset
  ///   Construct from adjacency lists
  Poset ( std::vector<std::vector<uint64_t>> & adjacencies );

  /// Poset
  ///   Construct from digraph
  Poset ( Digraph const& digraph );

  /// assign
  ///   Construct from adjacency lists
  void
  assign ( std::vector<std::vector<uint64_t>> & adjacencies );

  /// assign
  ///   Construct from digraph
  void
  assign ( Digraph const& digraph );

  /// size
  ///   Return number of vertices
  uint64_t
  size ( void ) const;

  /// parents
  ///   Return vector of vertices which are the parents of v
  ///   in the Hasse diagram of the poset
  std::vector<uint64_t> const&
  parents ( uint64_t v ) const;

  /// children
  ///   Return vector of vertices which are the children of v
  ///   in the Hasse diagram of the poset
  std::vector<uint64_t> const&
  children ( uint64_t v ) const;

  /// ancestors
  ///   Return vector of vertices which are the ancestors of v
  std::vector<uint64_t> const&
  ancestors ( uint64_t v ) const;

  /// descendants
  ///   Return vector of vertices which are the descendants of v
  std::vector<uint64_t> const&
  descendants ( uint64_t v ) const;

  /// maximal
  ///   Given a collection of elements, return the subset of those elements
  ///   which are maximal in the set
  std::set<uint64_t>
  maximal ( std::set<uint64_t> const& elements ) const;

  /// compare
  ///   Check if vertex u < vertex v in poset
  bool
  compare ( const uint64_t & u, const uint64_t & v ) const;

  /// permute
  ///   Reorder the digraph according to the provided permutation
  ///   The convention on the permutation is that vertex v in the input 
  ///   corresponds to vertex permutation[v] in the output
  Poset
  permute ( const std::vector<uint64_t> & permutation ) const;

  /// stringify
  ///   Return a JSON description
  std::string
  stringify ( void ) const;

  /// parse
  ///   Initialize from a JSON description
  ///   Format: A JSON description of a Digraph object
  ///           The digraph is one suitable for use in
  ///           the constructor method.
  void
  parse ( std::string const& str );

  /// graphviz
  ///   Return a graphviz representation of the poset (as Hasse diagram)
  std::string
  graphviz ( void ) const;
  
  /// operator <<
  ///   Emit data to stream in graphviz format
  friend std::ostream& operator << ( std::ostream& stream, Poset const& poset );

private:
  std::shared_ptr<Poset_> data_;
  /// serialize
  ///   For use with BOOST Serialization library,
  ///   which is used by the cluster-delegator MPI package
  friend class boost::serialization::access;
  template<class Archive>
  void serialize(Archive & ar, const unsigned int version) {
    ar & data_;
  }
};

struct Poset_ {
  Digraph transitive_closure;
  Digraph transitive_reduction;
  Digraph transpose_transitive_closure;
  Digraph transpose_transitive_reduction;
  std::vector<std::unordered_set<uint64_t>> relation;
  /// serialize
  ///   For use with BOOST Serialization library,
  ///   which is used by the cluster-delegator MPI package
  friend class boost::serialization::access;
  template<class Archive>
  void serialize(Archive & ar, const unsigned int version) {
    ar & transitive_closure;
    ar & transitive_reduction;
    ar & transpose_transitive_closure;
    ar & transpose_transitive_reduction;
    ar & relation;
  }
};

#endif
