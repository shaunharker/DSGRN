#include "PatternMatch.hpp"
#include <iostream>

int main() {
	wallgraphvector wallgraph;

	wallStruct wall0 = { {5,6}, {{'M'},{'I'},{'I'}} };
	wallStruct wall1 = { {2,4},  {{'I','M','D','m'},{'D'},{'D'}} };
	wallStruct wall2 = { {0,3}, {{'m','M'},{'D','I'},{'D'}} };
	wallStruct wall3 = { {0,1}, {{'I','m'},{'D'},{'D'}} };
	wallStruct wall4 = { {0,6}, {{'I'},{'I'},{'I','M'}} };
	wallStruct wall5 = { {3,4}, {{'D'},{'I','m'},{'D'}} };
	wallStruct wall6 = { {0,1,2,5}, {{'I'},{'I','m'},{'D'}} };

	for (auto w : { wall0, wall1, wall2, wall3, wall4, wall5, wall6 } ) {
		wallgraph.push_back(w);
	}

	PatternMatch PM ( wallgraph );

	std::cout << "Test 1: \n";
	std::list<patternvector> patterns = { { "MII","DmD","MDD" } };
	PatternMatch::resultslist results = PM.patternMatch( patterns, 3 );

	for (auto keypair : results ) {
			for ( auto str : keypair.first ) {
				std::cout << str << " ";
			}
			std::cout << "has " << keypair.second << " match(es)\n";		
	}
		// should match 0 5 3 1
	std::cout << "\n";

	// std::cout << "Test 2: \n";
	// pattern = { "IIM","ImD","MII","DmD","MDD","IIM" };
	// initial = 4;
	// std::cout << patternMatch( initial, pattern, wallgraph ) << "\n\n";
	// // should match 4 6 0 5 3 1 4

	// std::cout << "Test 3: \n";
	// pattern = { "DmD","MII","MID","MDD" };
	// initial = 5;
	// std::cout << patternMatch( initial, pattern, wallgraph ) << "\n";
	// // should match 5 4 0 6 2 3 1
} 