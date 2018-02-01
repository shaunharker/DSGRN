/// Wall.h
/// Shaun Harker
/// 2015-05-27

#pragma once

#include "common.h"

#include "Phase/Domain.h"

class Wall {
public:
  /// Wall 
  ///   Default constructor
  Wall ( void );

  /// Wall_
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

  /// operator <<
  ///   Output to stream
  friend std::ostream& operator << ( std::ostream& stream, Wall const& w );

private:
  uint64_t index_;
};
