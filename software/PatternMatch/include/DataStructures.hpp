#ifndef PATTERNMATCH_DATASTRUCTURES_HPP
#define PATTERNMATCH_DATASTRUCTURES_HPP

#include <String>
#include <list>
#include <vector>

const int DIMENSION;

struct patternElement {
	std::string extremum;
	std::string intermediate;
};

typedef std::list<patternElement> patternlist; 

struct wallStruct {
	std::list<uint64_t> outedges;
	std::list<uint64_t> inedges;
	double[DIMENSION] phasespace;
	int[DIMENSION] outsigns;
	int[DIMENSION] insigns;
	std::list<std::string> walllabels;
	int var_affected;
};

typedef std::vector<wallStruct> wallgraphvector;

// use Shaun's code to get domain graph

#endif