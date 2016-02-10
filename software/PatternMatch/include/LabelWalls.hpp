#ifndef PATTERNMATCH_WALLLABELS_HPP
#define PATTERNMATCH_WALLLABELS_HPP

#include "DataStructures.hpp"

void constructDiffs ( wallStruct* wall, wallgraphvector* wgptr, bool out );
std::list<std::string> appendToStringInList ( const std::list<std::string> labels, std::list<std::string> addme );
void FTable ( wallStruct* wall );
void makeWallLabels ( wallgraphvector* wgptr ); 

#endif
