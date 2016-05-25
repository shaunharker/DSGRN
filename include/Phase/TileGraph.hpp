/// TileGraph.hpp
/// Shaun Harker
/// 2016-05-25

#ifndef DSGRN_TILEGRAPH_HPP
#define DSGRN_TILEGRAPH_HPP

#ifndef INLINE_IF_HEADER_ONLY
#define INLINE_IF_HEADER_ONLY
#endif

#include "TileGraph.h"

INLINE_IF_HEADER_ONLY TileGraph::
TileGraph ( void ) {
  data_ . reset ( new TileGraph_ );
}

INLINE_IF_HEADER_ONLY TileGraph::
TileGraph ( Parameter const parameter ) {
  assign ( parameter );
}

INLINE_IF_HEADER_ONLY void TileGraph::
assign ( Parameter const parameter ) {
  int const Up = 1;
  int const Down = -1;
  data_ -> digraph_ = Digraph ();
  data_ -> parameter_ = parameter;
  uint64_t D = parameter . network () . size ();
  data_ -> dimension_ = D;
  Digraph & digraph = data_ -> digraph_;
  // Enumerate tiles
  std::unordered_map<Cell, uint64_t> tile_index;
  // Construct all cells
  // Note: this creates some spurious boundary cells, 
  //       but the "snoussi" method will discard them anyway
  std::vector<Cell> cells;
  uint64_t N = 1 << D;
  for ( Domain domain = Domain  ( parameter . network () . limits () ); isValid ( domain ); ++ domain ) {
    for ( int shape = 0; shape < N; ++ shape ) {
      cells . push_back ( Cell ( domain, shape ) );
    }
  }
  for ( auto const& cell : cells ) {
    // Check if cell is a tile; if not a tile, skip the cell
    if ( not parameter . snoussi ( cell ) ) continue;
    auto signed_permutation = parameter . hyperoctahedral ( cell );
    // Check if signed_permutation is an involution
    bool is_involution = true;
    for ( auto const& pair : signed_permutation ) {
      uint64_t x = pair . first;
      uint64_t y = pair . second . first;
      bool sign = pair . second . second;
      if ( x == y ) continue;
      if ( y == -1 || signed_permutation [ y ] != { x, sign } ) {
        is_involution = false;
        break;
      }
    }
    if ( is_involution ) {
      tile_index [ cell ] = tile_index . size ();
      digraph . add_vertex ();
    }
  }  


  // Loop through tiles and create adjacencies
  for ( auto const& cell_and_index : tile_index ) {
    Cell const& cell = cell_and_index . first;
    uint64_t const& index = cell_and_index . second;
    for ( uint64_t x = 0; x < D; ++ x ) {
      auto handle_boundary_adjacency = [&] ( int direction ) {
        // If no flow in this direction, do not build an adjacency
        if ( not parameter . absorbing ( cell, x, direction ? 1 : -1 ) ) return;
        //  Collapse in the direction of the flow onto a boundary cell
        Cell boundary_cell = cell . collapse ( { {x, direction} } );
        //  This might fail if the original cell was unbounded, so we check that.
        if ( not boundary_cell ) return;
        //  If the boundary cell is a tile, there is no need to make an adjacency.
        if ( tile_index . count ( boundary_cell ) ) return;
        //  We compute the adjacent cell on the other side of the boundary.
        Cell adjacent_cell = boundary_cell . expand ( { {x, direction} } );
        //  If we jumped across the 0th threshold, there is no adjacent cell, so we check that.
        if ( not adjacent_cell ) return;
        //  We create the edge.
        uint64_t adjacent_index = tile_index [ adjacent_cell ];
        digraph . add_edge ( index, adjacent_index );
      }
      if ( signed_permutation . count ( x ) == 0 ) {
        // Handle boundary adjacencies:
        handle_boundary_adjacency ( Down );
        handle_boundary_adjacency ( Up );
      } else {
        // Handle coboundary adjacencies
        //   Since the signed permutation is an involution (x^2 = id)
        //   it must be a composition of 4 types of disjoint cycles:
        //   x -> x +
        //   x -> x -
        //   x -> y +  and y -> x +
        //   x -> y -  and y -> x -
        // We create adjacencies according to this decomposition
        uint64_t y = signed_permutation[x].first;
        bool sign = signed_permutation[x].second;
        if ( y < x ) continue; // Don't add edges twice
        if ( signed_permutation[x].first == x ) {
          // 1D case
          Cell left_cell = cell . expand ( { {x, Down} });
          Cell right_cell = cell . expand ( { {x, Up} });
          uint64_t left_index = tile_index [ left_cell ];
          uint64_t right_index = tile_index [ right_cell ];
          if ( sign ) {
            digraph . add_edge ( index, left_index );
            digraph . add_edge ( index, right_index );
          } else {
            digraph . add_edge ( left_index, index );
            digraph . add_edge ( right_index, index ); 
          }
        } else {
          // 2D case
          Cell ne_cell = cell . expand ( { {x, Up}, {y, Up} } );
          Cell nw_cell = cell . expand ( { {x, Down}, {y, Up} } );
          Cell sw_cell = cell . expand ( { {x, Down}, {y, Down} } );
          Cell se_cell = cell . expand ( { {x, Up}, {y, Down} } );
          uint64_t ne_index = tile_index [ ne_cell ];
          uint64_t nw_index = tile_index [ nw_cell ];
          uint64_t sw_index = tile_index [ sw_cell ];
          uint64_t se_index = tile_index [ se_cell ];
          if ( sign ) {
            digraph . add_edge ( nw_index, index );
            digraph . add_edge ( se_index, index );
            digraph . add_edge ( index, ne_index );
            digraph . add_edge ( index, sw_index );
          } else {
            digraph . add_edge ( index, nw_index );
            digraph . add_edge ( index, se_index );
            digraph . add_edge ( ne_index, index );
            digraph . add_edge ( sw_index, index );
          }
        }
      }
    }
  }
  digraph . finalize ();
}

INLINE_IF_HEADER_ONLY Digraph const TileGraph::
digraph ( void ) const {
  return data_ -> digraph_;
}

INLINE_IF_HEADER_ONLY uint64_t TileGraph::
dimension ( void ) const {
  return data_ -> dimension_;
}

INLINE_IF_HEADER_ONLY Annotation const TileGraph::
annotate ( Component const& vertices ) const {
  Annotation a;
  // TODO
  return a;
}

INLINE_IF_HEADER_ONLY std::string TileGraph::
graphviz ( void ) const {
  return digraph () . graphviz (); 
}

INLINE_IF_HEADER_ONLY std::ostream& operator << ( std::ostream& stream, TileGraph const& dg ) {
  stream << dg . digraph ();
  return stream;
}

#endif
