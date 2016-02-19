#ifndef PATTERNMATCH_DATASTRUCTURES_HPP
#define PATTERNMATCH_DATASTRUCTURES_HPP

#include <boost/unordered_map.hpp> // boost has hash for std::pair
#include <String>
#include <list>
#include <vector>
#include <set>
// #include "DSGRN.h"

typedef std::vector<std::string> patternvector; 
typedef std::vector<std::set<char>> labelset;

struct wallStruct {
	std::list<uint64_t> outedges;
	labelset labels;
};

typedef std::vector<wallStruct> wallgraphvector;

// Shaun will provide implementation of wall graph that I can label

#endif