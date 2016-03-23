/// WallGraph.hpp
/// Shaun Harker
/// 2015-05-24

#ifndef DSGRN_WALLGRAPH_HPP
#define DSGRN_WALLGRAPH_HPP

#ifndef INLINE_IF_HEADER_ONLY
#define INLINE_IF_HEADER_ONLY
#endif

#include "WallGraph.h"

INLINE_IF_HEADER_ONLY WallGraph::
WallGraph ( void ) {
  data_ . reset ( new WallGraph_ );
}

INLINE_IF_HEADER_ONLY WallGraph::
WallGraph ( Parameter const parameter ) {
  assign ( parameter );
}

INLINE_IF_HEADER_ONLY void WallGraph::
assign ( Parameter const parameter ) {
  data_ . reset ( new WallGraph_ );

  data_ -> wall_index_to_vertex_ . clear ();
  data_ -> vertex_to_dimension_ . clear ();
  data_ -> parameter_ = parameter;
  int D = parameter . network() . size ();
  std::vector<uint64_t> limits = parameter . network() . domains ();
  data_ -> digraph_ = Digraph ();
  // Make wall indices
  for ( Domain dom (limits); dom.isValid(); ++ dom ) {
    for ( int d = 0; d < D; ++ d ) {
      if ( not dom . isMin(d) ) {
        Wall wall ( dom, d, -1 );
        uint64_t v = data_ -> digraph_ . add_vertex ();
        data_ -> wall_index_to_vertex_ [ wall.index() ] = v;
        //std::cout << "Created wall with index " << wall.index() << " = vertex " << v << "\n";
        data_ -> vertex_to_dimension_ . push_back ( d );
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
        if ( parameter . absorbing ( dom, d, -1 ) ) {
          absorbing . push_back ( wall );
        } else {
          entrance . push_back ( wall );
        }
      }
      if ( not dom . isMax(d) ) {
        Wall wall ( dom, d, 1 );
        //std::cout << "Wallcheck. Right wall (dim " << d << ") of domain "
        //          << dom.index() << " is wall " << wall.index() << "\n";
        if ( parameter . absorbing ( dom, d, 1 ) ) {
          absorbing . push_back ( wall );
        } else {
          entrance . push_back ( wall );
        }
      }
    }
    for ( Wall const& x : entrance ) {
      for ( Wall const& y : absorbing ) {
        uint64_t i = data_ -> wall_index_to_vertex_ [ x.index() ];
        uint64_t j = data_ -> wall_index_to_vertex_ [ y.index() ];
        //std::cout << "Adding wall-wall edge " << i << " -> " << j << "\n";
        data_ -> digraph_ . add_edge ( i, j );
      }
    }
    if ( absorbing . empty () ) {
      uint64_t attract = data_ -> digraph_ . add_vertex ();
      data_ -> digraph_ . add_edge ( attract, attract );
      data_ -> vertex_to_dimension_ . push_back ( D );
      // (annotations wants to know how many variables pass 1st threshold
      // for domains)
      for ( int d = 0; d < D; ++ d ) {
        if ( dom [ d ] > 0 ) ++ data_ -> vertex_to_dimension_ [ attract ];
      }
      for ( Wall const& x : entrance ) {
        uint64_t i = data_ -> wall_index_to_vertex_ [ x.index() ];
        //std::cout << "Adding wall-domain edge " << i << " -> " << attract << "\n";
        data_ -> digraph_ . add_edge ( i, attract );
      }
    }
  }
  data_ -> digraph_ . finalize ();
}

INLINE_IF_HEADER_ONLY Digraph const WallGraph::
digraph ( void ) const {
  return data_ ->digraph_;
}

INLINE_IF_HEADER_ONLY Annotation const WallGraph::
annotate ( Component const& vertices ) const {
  uint64_t D = data_ -> parameter_ . network() . size ();
  std::set<uint64_t> signature;
  bool all_on = true;
  bool all_off = true;
  for ( uint64_t v : vertices ) {
    uint64_t d = data_ -> vertex_to_dimension_ [ v ];
    if ( d < D ) signature . insert ( d );
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
      ss << data_ ->parameter_ . network() . name ( d );
    }
    ss << "}";
  }
  a . append ( ss . str () );
  return a;
}

INLINE_IF_HEADER_ONLY std::ostream& operator << ( std::ostream& stream, WallGraph const& dg ) {
  stream << dg . digraph ();
  return stream;
}


#endif
