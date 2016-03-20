#include "DataStructures.hpp"
#include <iostream>

int main() {
	// make wall graph
	wallStruct wall1;
	wall1.outedges = {5,6}; 
	wall1.walllabels = {"Muu"};

	wallStruct wall2;
	wall2.outedges = {2,4,6,7}; 
	wall2.walllabels = {"udd","Mdd","ddd","mdd"};

	wallgraphvector wg = {wall1, wall2};

	// use iterators
	std::vector<wallStruct>::iterator it;
	std::list<uint64_t>::iterator ut;
	std::list<std::string>::iterator lt;

	// collect results
	bool testresult = true;
}