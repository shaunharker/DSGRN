/// PatternMatch.h
/// Shaun Harker and Bree Cummins
/// 2016-03-19

#ifndef PATTERNMATCH_H
#define PATTERNMATCH_H

#include "PatternGraph.h"

/// QueryCycleMatch
///   Determine if there is a cycle match
bool
QueryCycleMatch ( MatchingGraph const& mg );

/// QueryCycleMatch
///   Determine if there is a path match
bool
QueryPathMatch ( MatchingGraph const& mg );

/// CountCycleMatches
///   Return the number of cycle matches
uint64_t
CountCycleMatches ( MatchingGraph const& mg );

/// CountPathMatches
///   Return the number of path matches
uint64_t
CountPathMatches ( MatchingGraph const& mg );

#endif
