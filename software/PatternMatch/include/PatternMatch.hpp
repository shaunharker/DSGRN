#ifndef PATTERNMATCH_HPP
#define PATTERNMATCH_HPP

#include "DataStructures.hpp"
#include <algorithm>
#include <iostream>

void printMatch( std::list<uint64_t> match ); 
bool recursePattern_withmatch (uint64_t currentwallindex, patternlist pattern, wallgraphvector* wallgraphptr, std::list<uint64_t> match = {});
bool recursePattern (uint64_t currentwallindex, patternlist pattern, wallgraphvector* wallgraphptr );
 
 #endif
