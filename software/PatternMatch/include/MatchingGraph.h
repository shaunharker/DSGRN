/// MatchingGraph.h
/// Shaun Harker and Bree Cummins
/// 2016-03-19

#ifndef MATCHINGGRAPH_H
#define MATCHINGGRAPH_H

#include <boost/functional/hash.hpp>

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

  /// Vertex
  ///   vertex data type. The first entry correspond to a vertex
  ///   in the search graph (which we call a domain), and the
  ///   second entry is the pattern graph index, which we call
  ///   position 
  typedef std::pair<uint64_t,uint64_t> Vertex;

  /// adjacencies
  ///   Given a vertex v, return vector of vertices which 
  ///   are out-edge adjacencies of input v
  std::vector<Vertex> const&
  adjacencies ( uint64_t v ) const;

  /// domain
  ///   Given a vertex v, return the associated domain (i.e. vertex in search graph)
  ///   Note this domain index may not be consistent with the indexing in the domain graph
  ///   since we have taken a subgraph
  uint64_t
  domain ( Vertex const& v ) const;

  /// position
  ///   Given a vertex v, return the associated position (i.e. vertex in the pattern graph)
  uint64_t 
  position ( Vertex const& v ) const;

  /// vertex
  ///   Given a domain and position, return the associated vertex in the matching graph
  Vertex
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
  PatternGraph pg_;
  SearchGraph sg_;
  /// serialize
  ///   For use with BOOST Serialization library,
  ///   which is used by the cluster-delegator MPI package
  friend class boost::serialization::access;
  template<class Archive>
  void serialize(Archive & ar, const unsigned int version) {
    ar & pg_;
    ar & sg_;
  }
};

#endif
