#ifndef PATTERNMATCH_HPP
#define PATTERNMATCH_HPP

#include "DataStructures.hpp"
#include <algorithm>
#include <iostream>
#include <stack>

class PatternMatch {
	
	typedef std::pair<uint64_t, patternlist> wallpair;

	public:
		bool patternMatch ( );
		// add results bool and data structure

	private:
		patternlist startpattern;
		const wallgraphvector wallgraph;
		const bool findonlyone;
		const std::list<uint64_t> startwall;
		boost::unordered_map<wallpair, bool> memoize; // recurrence checker, data storage
		std::stack<wallpair> nodes_to_visit; // stack replacement for recursion
		nodes_to_visit.push_front( std::make_pair(startwall, startpattern) );
		bool _checkForWordInLabels( const std::string word, const std::vector<std::set<char>> labels);
		void _addToStack ( const patternlist p, const wallpair key );
		void _saveDataStructure ( ) = { ; }; // unhash memoize and save only true paths
		patternlist _makePatternFromKey( wallpair );

};

#endif
