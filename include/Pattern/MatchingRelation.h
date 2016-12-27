/// MatchingRelation.h
/// Shaun Harker
/// MIT LICENSE 
/// 2016-12-26

#ifndef MATCHINGRELATION_H
#define MATCHINGRELATION_H

#include "common.h"

/// MatchingRelation
///   In principle this should have matching functionality
///   but currently this is handled in different ways. A
///   future refactor will likely expand this class.
///   Currently it gives provides methods to make labels
///   human-readable.
class MatchingRelation {
public:
  /// MatchingRelation
  ///   Default constructor
  MatchingRelation ( void );

  /// MatchingRelation
  ///   Construct matching relation given dimension "dim"
  MatchingRelation ( uint64_t dim );

  /// assign
  ///   Construct matching relation given dimension "dim"
  void
  assign ( uint64_t dim );

  /// dimension
  uint64_t
  dimension ( void ) const;

  /// vertex_labelstring
  std::string
  vertex_labelstring ( uint64_t label ) const;

  /// edge_labelstring
  std::string
  edge_labelstring ( uint64_t label ) const;  

private:
  uint64_t dimension_;
};

#endif
