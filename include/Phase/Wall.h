/// Wall.h
/// Shaun Harker
/// 2015-05-27

#ifndef DSGRN_WALL_H
#define DSGRN_WALL_H

#include <vector>
#include <cstdlib>

class Wall {
public:
  /// Wall 
  ///   Default constructor
  Wall ( void );

  /// Wall
  ///   Construct from domain collapse (see assign)
  Wall ( Domain const& dom, uint64_t collapse_dim, int direction );

  /// assign 
  ///   Create wall from domain, dimension of collapse, 
  ///   and direction of collapse.
  void
  assign ( Domain const& dom, uint64_t collapse_dim, int direction );

  /// index
  ///   Return a unique index. Note wall indices are 
  ///   not guaranteed to be contiguous.
  uint64_t
  index ( void ) const;

private:
  uint64_t index_;
};

inline Wall::
Wall ( void ) {}

inline Wall::
Wall ( Domain const& dom, uint64_t collapse_dim, int direction ) {
  assign ( dom, collapse_dim, direction );
}

inline void Wall::
assign ( Domain const& dom, uint64_t collapse_dim, int direction ) {
  uint64_t dom_index = (direction == 1) ? dom . right (collapse_dim) : dom . index ();
  index_ = (dom_index << dom . size ()) | (1LL << collapse_dim);
}
  
inline uint64_t Wall::
index ( void ) const {
  return index_;
}

#endif
