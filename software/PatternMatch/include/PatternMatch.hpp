#ifndef PATTERNMATCH_HPP
#define PATTERNMATCH_HPP

#include "DataStructures.hpp"
#include <algorithm>
#include <iostream>
#include <stack>

class PatternMatch {
	
	typedef std::pair<uint64_t,uint64_t> node;
	typedef boost::unordered_map< node, uint64_t > memoize;

	public:
		std::list<std::pair<patternlist,uint64_t>> patternMatch ( std::list<patternvector> allpatterns ); // loop over patterns; patterns are either already sorted in reverse order, or the input should be a partial order; can I do sorting during construction from a partial order? (topological sorting)

		// queries for periodic pattern and find option (have help string for find option)
		
	private:
		const wallgraphvector wallgraph;
		const int findoption; 	// 1 = find at most one match over all patterns (check subset of patterns)
								// 2 = find at most one match per pattern (check all patterns)
								// 3 = find number of matches for every pattern (check all patterns)
		const bool periodicpattern; // FIXME: IMPLEMENT THIS IN PATTERN MATCH

		std::stack<node> _initializeStack ( patternvector pattern ); 
									// std::stack<wallpair> nodes_to_visit; make startwalls; 			 		for s in startwall; make wallpair (s, pattern); nodes_to_visit.push_front(wallpair); return nodes_to_visit
		bool _checkForWordInLabels( const std::string word, const std::vector<std::set<char>> labels);
		void _addToStack ( const patternvector p, const node key );
		bool _patternMatch ( patternvector pattern, memoize& keepcount );

};

#endif
