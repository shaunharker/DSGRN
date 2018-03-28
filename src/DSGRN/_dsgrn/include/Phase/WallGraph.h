/// WallGraph.h
/// Shaun Harker
/// 2015-05-24

#pragma once 

#include "common.h"

#include "Parameter/Parameter.h"
#include "Graph/Digraph.h"
#include "Graph/Components.h"
#include "Dynamics/Annotation.h"

struct WallGraph_;

class WallGraph : public TypedObject {
public:

  virtual std::string type (void) const final { return "WallGraph"; }

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
};

struct WallGraph_ {
  Digraph digraph_;
  Parameter parameter_;
  std::unordered_map<uint64_t, uint64_t> wall_index_to_vertex_;
  std::vector<uint64_t> vertex_to_dimension_;
};
