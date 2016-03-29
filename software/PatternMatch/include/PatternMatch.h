/// PatternMatch.h
/// Shaun Harker and Bree Cummins
/// 2016-03-19

#ifndef PATTERNMATCH_H
#define PATTERNMATCH_H

#include "DSGRN.h"
#include "common.h"
#include "MatchingGraph.h"

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

/// Debugging

/// ExplainCycleMatch
///   Return a path through the MatchingGraph corresponding 
///   to a cycle match
std::string
ExplainCycleMatch ( MatchingGraph const& mg );

#endif
