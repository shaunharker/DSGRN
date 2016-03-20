/// MatchingGraph.h
/// Shaun Harker and Bree Cummins
/// 2016-03-19

#ifndef MATCHINGGRAPH_H
#define MATCHINGGRAPH_H

#include "PatternGraph.h"
#include "SearchGraph.h"

struct MatchingGraph_;

class MatchingGraph {
public:
  /// MatchingGraph
  ///   Default constructor
  MatchingGraph ( void );

  /// MatchingGraph
  ///   Create a matching graph from a pattern graph and a search graph
  MatchingGraph ( PatternGraph const& pg, SearchGraph const& sg );

  /// assign
  ///   Create a matching graph from a pattern graph and a search graph
  void
  assign ( PatternGraph const& pg, SearchGraph const& sg );

  /// PatternGraph
  ///    Return associated pattern graph
  PatternGraph const&
  PatternGraph ( void ) const;

  /// SearchGraph
  ///   Return associated search graph
  SearchGraph const&
  SearchGraph ( void ) const;

  /// size
  ///   Return the number of vertices in the matching graph
  uint64_t
  size ( void ) const;

  /// adjacencies
  ///   Given a vertex v, return vector of vertices which 
  ///   are out-edge adjacencies of input v
  std::vector<uint64_t> const&
  adjacencies ( uint64_t v ) const;

  /// domain
  ///   Given a vertex v, return the associated domain (i.e. vertex in search graph)
  uint64_t
  domain ( uint64_t v ) const;

  /// position
  ///   Given a vertex v, return the associated position (i.e. vertex in the pattern graph)
  uint64_t 
  position ( uint64_t v ) const;

  /// vertex
  ///   Given a domain and position, return the associated vertex in the matching graph
  uint64_t
  vertex ( uint64_t domain, uint64_t position ) const;

private:
  std::shared_ptr<MatchingGraph_> data_;
  /// serialize
  ///   For use with BOOST Serialization library,
  ///   which is used by the cluster-delegator MPI package
  friend class boost::serialization::access;
  template<class Archive>
  void serialize(Archive & ar, const unsigned int version) {
    ar & data_;
  }
};

struct MatchingGraph_ {
  
  // TODO: determine data required.

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
