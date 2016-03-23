/// MatchingGraph.h
/// Shaun Harker and Bree Cummins
/// 2016-03-19

#ifndef MATCHINGGRAPH_H
#define MATCHINGGRAPH_H

#include "DSGRN.h"
#include "common.h"
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
  ///   Create a matching graph from a search graph and a pattern graph
  MatchingGraph ( SearchGraph const& sg, PatternGraph const& pg );

  /// assign
  ///   Create a matching graph from a search graph and a pattern graph
  void
  assign ( SearchGraph const& sg, PatternGraph const& pg );

  /// SearchGraph
  ///   Return associated search graph
  SearchGraph const&
  searchgraph ( void ) const;

  /// PatternGraph
  ///   Return associated pattern graph
  PatternGraph const&
  patterngraph ( void ) const;

  /// Vertex
  ///   vertex data type. The first entry correspond to a vertex
  ///   in the search graph (which we call a domain), and the
  ///   second entry is the pattern graph index, which we call
  ///   position 
  typedef std::pair<uint64_t,uint64_t> Vertex;

  /// query
  ///   Given a (domain, position) pair, determine if it is
  ///   a vertex in the matching graph
  bool
  query ( Vertex const& v ) const;
  
  /// adjacencies
  ///   Given a vertex v, return vector of vertices which 
  ///   are out-edge adjacencies of input v
  std::vector<Vertex>
  adjacencies ( Vertex const& v ) const;

  /// roots
  ///   Give the elements of the form (domain, root)
  ///   in the matching graph
  std::vector<Vertex>
  roots ( void ) const;
  
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
  ///   Note: will 
  Vertex
  vertex ( uint64_t domain, uint64_t position ) const;

  /// graphviz
  ///   Return a graphviz representation of the matching graph
  std::string
  graphviz ( void ) const;

private:
  /// _match
  ///   Return true if search graph label and pattern graph label match
  bool
  _match ( uint64_t search_label, uint64_t pattern_label ) const;

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
