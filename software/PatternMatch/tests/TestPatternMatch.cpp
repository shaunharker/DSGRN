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
	std::list<patternvector> patterns;
	PatternMatch::resultsvector results;
	std::vector<uint64_t> answer;

	// std::cout << "Test 1: \n";
	// patterns = { { "MII","DmD","MDD" } };
	// std::cout << ( PM.patternMatch( patterns, 1 ).front().second==1 ) << " " << ( PM.patternMatch( patterns, 2 ).front().second==1 ) << " " << ( PM.patternMatch( patterns, 3 ).front().second==2 ) << "\n";
	// // should match 0 5 3 1, 0 5 3 1 2

	// std::cout << "Test 2: \n";
	// patterns = { { "IIM","ImD","MII","DmD","MDD","IIM" } };
	// std::cout << ( PM.patternMatch( patterns, 1 ).front().second==1 ) << " " << ( PM.patternMatch( patterns, 2 ).front().second==1 ) << " " << ( PM.patternMatch( patterns, 3 ).front().second== 1 ) << "\n";
	// // should match 4 6 0 5 3 1 4

	// std::cout << "Test 3: \n";
	// patterns = { { "DmD","MII","MID","MDD" } };
	// std::cout << ( PM.patternMatch( patterns, 1 ).front().second==1 ) << " " << ( PM.patternMatch( patterns, 2 ).front().second==1 ) << " " << ( PM.patternMatch( patterns, 3 ).front().second== 2 ) << "\n";

	// std::cout << "Test 4: \n";
	// patterns = { { "MII","DmD","MDD" }, { "DmD","MII","MID","MDD" } };
	// results = PM.patternMatch( patterns, 2 );
	// answer = {1,1};

	// for (int i = 0; i < results.size(); ++i ) {
	// 	std::cout << ( results[ i ].second == answer[ i ] ) << " ";		
	// }
	// // should be 1 1
	// std::cout << "\n";

	// std::cout << "Test 5: \n";
	// patterns = { { "MII","DmD","MDD" }, { "DmD","MII","MID","MDD" } };
	// results = PM.patternMatch( patterns, 3 );
	// answer = {2,2};

	// for (int i = 0; i < results.size(); ++i ) {
	// 	std::cout << ( results[ i ].second == answer[ i ] ) << " ";		
	// }
	// // should be 1 1
	// std::cout << "\n";


	// std::cout << "Test 6: \n";
	// patterns = { { "IIM", "MII","ImD","mDD", "MII" }, { "MII","ImD","mDD", "MII" }, { "IIM", "ImD","mDD", "MII" } };
	// results = PM.patternMatch( patterns, 3 );
	// answer = {3,3,3};

	// for (int i = 0; i < results.size(); ++i ) {
	// 	std::cout << ( results[ i ].second == answer[ i ] ) << " ";		
	// }
	// std::cout << "\n";

	std::cout << "Test 7: \n";
	// patterns = { { "IIM", "MII","ImD","mDD", "MII" }, { "MII","ImD","mDD", "MII" }, { "IIM", "ImD","mDD", "MII" } };
	patterns = { { "MII","ImD","mDD", "MII" }, { "IIM", "ImD","mDD", "MII" } };
	results = PM.patternMatch( patterns, 2 );
	answer = {1,1};

	for (int i = 0; i < results.size(); ++i ) {
		std::cout << ( results[ i ].second == answer[ i ] ) << " ";		
	}
	std::cout << "\n";

	// for (auto keypair : results ) {
	// 	for ( auto str : keypair.first ) {
	// 		std::cout << str << " ";
	// 	}
	// 	std::cout << "has " << keypair.second << " match(es)\n";		
	// }
	// // should be 2 2
	// std::cout << "\n";

} 