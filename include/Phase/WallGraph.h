/// WallGraph.h
/// Shaun Harker
/// 2015-05-24

#ifndef DSGRN_WALLGRAPH_H
#define DSGRN_WALLGRAPH_H

#include "common.h"

#include "Parameter/Parameter.h"
#include "Graph/Digraph.h"
#include "Graph/Components.h"
#include "Dynamics/Annotation.h"

struct WallGraph_;

class WallGraph {
public:
  /// WallGraph
  WallGraph ( void );

  /// WallGraph
  ///   Construct based on parameter and network
  WallGraph ( Parameter const parameter );

  /// assign
  ///   Construct based on parameter and network
  void
  assign ( Parameter const parameter );

  /// digraph
  ///   Return underlying digraph
  Digraph const
  digraph ( void ) const;  

  /// annotate
  Annotation const
  annotate ( Component const& vertices ) const;

  /// operator <<
  ///   Emit data to stream in graphviz format
  friend std::ostream& operator << ( std::ostream& stream, WallGraph const& dg );

private:
  std::shared_ptr<WallGraph_> data_;
  /// serialize
  ///   For use with BOOST Serialization library,
  ///   which is used by the cluster-delegator MPI package
  friend class boost::serialization::access;
  template<class Archive>
  void serialize(Archive & ar, const unsigned int version) {
    ar & data_;
  }
};

struct WallGraph_ {
  Digraph digraph_;
  Parameter parameter_;
  std::unordered_map<uint64_t, uint64_t> wall_index_to_vertex_;
  std::vector<uint64_t> vertex_to_dimension_;
  /// serialize
  ///   For use with BOOST Serialization library,
  ///   which is used by the cluster-delegator MPI package
  friend class boost::serialization::access;
  template<class Archive>
  void serialize(Archive & ar, const unsigned int version) {
    ar & digraph_;
    ar & parameter_;
    ar & wall_index_to_vertex_;
    ar & vertex_to_dimension_;
  }
};

#endif
