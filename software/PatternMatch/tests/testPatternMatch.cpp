#include "PatternMatch.hpp"

int main() {
	wallgraphvector wallgraph;
	wallgraphvector *wallgraphptr = &wallgraph;

	wallStruct wall0 = { {5,6}, {"Muu"} };
	wallStruct wall1 = { {2,4}, {"udd","Mdd","ddd","mdd"} };
	wallStruct wall2 = { {0,3}, {"mdd"} };
	wallStruct wall3 = { {0,1}, {"ddd","mdd"} };
	wallStruct wall4 = { {6}, {"uuM","uuu"} };
	wallStruct wall5 = { {3,4}, {"dmd","dud"} };
	wallStruct wall6 = { {0,1,2,5}, {"umd","uud"} };

	for (auto w : { wall0, wall1, wall2, wall3, wall4, wall5, wall6 } ) {
		wallgraph.push_back(w);
	}


	// // works
	// patternlist pattern = { {"Muu","uud"}, {"dmd","ddu"}, {"Mdd","ddd"} };
	// uint64_t initial = 0;
	// // should match 0 5 3 1

	// patternlist pattern = { {"uuM","uud"}, {"umd","udd"}, {"Muu","uud"}, {"dmd","ddu"}, {"Mdd","ddd"}, {"uuM","uud"} };
	// uint64_t initial = 4;
	// // should match 4 6 0 5 3 1 4

	patternlist pattern = { {"mdd","uud"}, {"Muu","udd"}, {"Muu","uud"}, {"dmd","ddu"}, {"umd","uuu"}, {"Mdd","uud"} };
	uint64_t initial = 2;
	// should match 2 0 6 0 5 4 6 1

	bool ismatch = recursePattern_withmatch( initial, pattern, wallgraphptr );

	std::cout << ismatch << "\n";


	return ismatch;

} 