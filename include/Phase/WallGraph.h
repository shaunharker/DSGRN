/// WallGraph.h
/// Shaun Harker
/// 2015-05-24

#ifndef DSGRN_WALLGRAPH_H
#define DSGRN_WALLGRAPH_H

#include <memory>
#include <vector>
#include <string>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include "Phase/Domain.h"
#include "Phase/Wall.h"
#include "Graph/Digraph.h"
#include "Dynamics/Annotation.h"

class WallGraph {
public:
  /// assign
  ///   Construct based on parameter and network
  void
  assign ( std::shared_ptr<Parameter const> parameter );

  /// digraph
  ///   Return underlying digraph
  std::shared_ptr<Digraph const>
  digraph ( void ) const;  

  /// annotate
  Annotation
  annotate ( std::vector<uint64_t> const& vertices ) const;

  /// operator <<
  ///   Emit data to stream in graphviz format
  friend std::ostream& operator << ( std::ostream& stream, WallGraph const& dg );

private:
  std::shared_ptr<Digraph> digraph_;
  std::shared_ptr<Parameter const> parameter_;
  std::unordered_map<uint64_t, uint64_t> wall_index_to_vertex_;
  std::vector<uint64_t> vertex_to_dimension_;
};

inline void WallGraph::
assign ( std::shared_ptr<Parameter const> parameter ) {
  wall_index_to_vertex_ . clear ();
  vertex_to_dimension_ . clear ();
  parameter_ = parameter;
  int D = parameter -> network() -> size ();
  std::vector<uint64_t> limits = parameter -> network() -> domains ();
  digraph_ . reset ( new Digraph );
  // Make wall indices
  for ( Domain dom (limits); dom.isValid(); ++ dom ) {
    for ( int d = 0; d < D; ++ d ) {
      if ( not dom . isMin(d) ) {
        Wall wall ( dom, d, -1 );
        uint64_t v = digraph_ -> add_vertex ();
        wall_index_to_vertex_ [ wall.index() ] = v;
        //std::cout << "Created wall with index " << wall.index() << " = vertex " << v << "\n";
        vertex_to_dimension_ . push_back ( d );
      }
    }
  }
  // Determine dynamics
  for ( Domain dom (limits); dom.isValid(); ++ dom ) {
    std::vector<Wall> entrance;
    std::vector<Wall> absorbing;
    for ( int d = 0; d < D; ++ d ) {
      if ( not dom . isMin(d) ) {
        Wall wall ( dom, d, -1 );
        //std::cout << "Wallcheck. Left wall (dim " << d << ") of domain " 
        //          << dom.index() << " is wall " << wall.index() << "\n";
        if ( parameter -> absorbing ( dom, d, -1 ) ) {
          absorbing . push_back ( wall );
        } else {
          entrance . push_back ( wall );
        }
      }
      if ( not dom . isMax(d) ) {
        Wall wall ( dom, d, 1 );
        //std::cout << "Wallcheck. Right wall (dim " << d << ") of domain " 
        //          << dom.index() << " is wall " << wall.index() << "\n";
        if ( parameter -> absorbing ( dom, d, 1 ) ) {
          absorbing . push_back ( wall );
        } else {
          entrance . push_back ( wall );
        }
      }
    }
    for ( Wall const& x : entrance ) {
      for ( Wall const& y : absorbing ) {
        uint64_t i = wall_index_to_vertex_ [ x.index() ];
        uint64_t j = wall_index_to_vertex_ [ y.index() ]; 
        //std::cout << "Adding wall-wall edge " << i << " -> " << j << "\n";       
        digraph_ -> add_edge ( i, j );
      }
    }
    if ( absorbing . empty () ) {
      uint64_t attract = digraph_ -> add_vertex ();
      vertex_to_dimension_ . push_back ( D );
      // (annotations wants to know how many variables pass 1st threshold
      // for domains)
      for ( int d = 0; d < D; ++ d ) {
        if ( dom [ d ] > 0 ) ++ vertex_to_dimension_ [ attract ];
      }
      for ( Wall const& x : entrance ) {
        uint64_t i = wall_index_to_vertex_ [ x.index() ];
        //std::cout << "Adding wall-domain edge " << i << " -> " << attract << "\n";       
        digraph_ -> add_edge ( i, attract );
      }
    }
  }
}

inline std::shared_ptr<Digraph const> WallGraph::
digraph ( void ) const {
  return digraph_;
}

inline Annotation WallGraph::
annotate ( std::vector<uint64_t> const& vertices ) const {
  uint64_t D = parameter_ -> network() -> size ();
  std::vector<uint64_t> signature;
  bool all_on = true;
  bool all_off = true;
  for ( uint64_t v : vertices ) {
    uint64_t d = vertex_to_dimension_ [ v ];
    if ( d < D ) signature . push_back ( d );
    if ( d > D ) all_off = false;
    if ( d < 2*D ) all_on = false; 
  }
  Annotation a;
  std::stringstream ss;
  if ( signature . size () == 0 ) {
    ss << "FP";
    if ( all_on ) ss << " ON";
    if ( all_off) ss << " OFF";
  } else if ( signature . size () == D ) {
    ss << "FC";
  } else {
    ss << "XC {";
    bool first_term = true;
    for ( uint64_t d : signature ) {
      if ( first_term ) first_term = false; else ss << ", ";
      ss << d;
    }
    ss << "}";
  }
  a . append ( ss . str () );
  return a;
}

inline std::ostream& operator << ( std::ostream& stream, WallGraph const& dg ) {
  stream << * dg . digraph ();
  return stream;
}


#endif
