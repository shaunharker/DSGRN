/// PatternGraph.h
/// Shaun Harker and Bree Cummins
/// 2016-03-19

#ifndef PATTERNGRAPH_H
#define PATTERNGRAPH_H

#include "DSGRN.h"
#include "common.h"
#include "Pattern.h"

struct PatternGraph_;

class PatternGraph {
public:
  /// PatternGraph
  ///   Default constructor
  PatternGraph ( void );

  /// PatternGraph
  ///   Create a pattern graph from a pattern
  PatternGraph ( Pattern const& pattern );

  /// assign
  ///   Create a pattern graph from a poset
  void
  assign ( Pattern const& pattern );

  /// root
  ///   Return the index of the root vertex
  uint64_t 
  root ( void ) const;

  /// leaf
  ///   Return the index of the leaf vertex
  uint64_t 
  leaf ( void ) const;

  /// size
  ///   Return the number of vertices in the pattern graph
  uint64_t
  size ( void ) const;

  /// dimension
  uint64_t
  dimension ( void ) const;

  /// label
  ///   Given a vertex, return the associated label
  ///   The label is a 64 bit word with bits interpreted as follows:
  ///    bit i+D     bit i   interpretation
  ///         0        1    ith variable decreasing  
  ///         1        0    ith variable increasing
  ///         0        0    ith variable can either increase or decrease
  ///   Note the limitation to 32 dimensions. Here D is the number of
  ///   dimensions.
  uint64_t
  label ( uint64_t v ) const;
  
  /// consume
  ///   Given a vertex v and a variable, report the unique vertex u, if it exists,
  ///   in the adjacency list of vertex. This corresponds to "consuming" the edge
  ///   in the pattern graph.
  uint64_t 
  consume ( uint64_t vertex, uint64_t variable ) const;

  /// graphviz
  ///   Return a graphviz representation of the pattern graph
  std::string
  graphviz ( void ) const;

private:
  std::shared_ptr<PatternGraph_> data_;
  /// serialize
  ///   For use with BOOST Serialization library,
  ///   which is used by the cluster-delegator MPI package
  friend class boost::serialization::access;
  template<class Archive>
  void serialize(Archive & ar, const unsigned int version) {
    ar & data_;
  }
};

struct PatternGraph_ {
  uint64_t root_;
  uint64_t leaf_;
  uint64_t size_;
  uint64_t dimension_;
  std::vector<uint64_t> labels_;
  std::vector<std::unordered_map<uint64_t, uint64_t>> consume_;

  /// serialize
  ///   For use with BOOST Serialization library,
  ///   which is used by the cluster-delegator MPI package
  friend class boost::serialization::access;
  template<class Archive>
  void serialize(Archive & ar, const unsigned int version) {
    ar & root_;
    ar & leaf_;
    ar & size_;
    ar & dimension_;
    ar & labels_;
    ar & consume_;
  }
};

#endif
