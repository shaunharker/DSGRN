#ifndef PATTERNMATCH_DATASTRUCTURES_HPP
#define PATTERNMATCH_DATASTRUCTURES_HPP

#include <boost/unordered_map.hpp>
#include <String>
#include <list>
#include <vector>
#include <set>
// #include "DSGRN.h"

const int DIMENSION = 2;

typedef std::vector<std::string> patternvector; 
typedef std::pair<uint64_t,patternlist> keypair;

struct wallStruct {
	std::list<uint64_t> outedges;
	std::vector<std::set<char>> labels;
};

typedef std::vector<wallStruct> wallgraphvector;

// Shaun will provide implementation of wall graph that I can label

#endif