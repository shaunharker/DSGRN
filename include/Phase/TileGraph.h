/// TileGraph.h
/// Shaun Harker
/// 2016-05-25

#ifndef DSGRN_TILEGRAPH_H
#define DSGRN_TILEGRAPH_H

#include "common.h"
#include "Parameter/Parameter.h"
#include "Graph/Digraph.h"
#include "Dynamics/Annotation.h"
#include "Graph/Components.h"

struct TileGraph_;

class TileGraph {
public:
  /// constructor
  TileGraph ( void );

  /// TileGraph
  ///   Construct based on parameter and network
  TileGraph ( Parameter const parameter );

  /// assign
  ///   Construct based on parameter and network
  void
  assign ( Parameter const parameter );

  /// digraph
  ///   Return underlying digraph
  Digraph const
  digraph ( void ) const;  

  /// dimension
  ///   Return dimension of phase space
  uint64_t
  dimension ( void ) const;

  /// annotate
  ///   Given a collection of vertices, return an
  ///   annotation describing them
  Annotation const
  annotate ( Component const& vertices ) const;

  /// graphviz
  ///   Return a graphviz representation of the domain graph
  std::string
  graphviz ( void ) const;

  /// operator <<
  ///   Emit data to stream in graphviz format
  friend std::ostream& operator << ( std::ostream& stream, TileGraph const& dg );

private:
  std::shared_ptr<TileGraph_> data_;
  /// serialize
  ///   For use with BOOST Serialization library,
  ///   which is used by the cluster-delegator MPI package
  friend class boost::serialization::access;
  template<class Archive>
  void serialize(Archive & ar, const unsigned int version) {
    ar & data_;
  }
};

struct TileGraph_ {
  uint64_t dimension_;
  Digraph digraph_;
  Parameter parameter_;

  /// serialize
  ///   For use with BOOST Serialization library,
  ///   which is used by the cluster-delegator MPI package
  friend class boost::serialization::access;
  template<class Archive>
  void serialize(Archive & ar, const unsigned int version) {
    ar & dimension_;
    ar & digraph_;
    ar & parameter_;
  }
};

#endif
