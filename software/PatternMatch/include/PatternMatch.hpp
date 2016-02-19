#ifndef PATTERNMATCH_HPP
#define PATTERNMATCH_HPP

#include "DataStructures.hpp"
#include <algorithm>
#include <iostream>
#include <stack>

class PatternMatch {
	
	public:
		// results data type: list< pair < pattern, number_matches > >
		typedef std::list<std::pair<patternvector,uint64_t>> resultslist; 

		// stack element data type: < wall_index, pattern_len > 
		typedef std::pair<uint64_t,uint64_t> node; 

		// data structure for memoization indexed by pattern length
		typedef std::pair<uint64_t, bool> keypair; // key = < wall_index, is_extremum > 
		typedef boost::unordered_map< keypair, int64_t> pathElementMap; // map = < key, count_of_paths >
		typedef std::vector<pathElementMap> memoize;

		// constructor
		PatternMatch ( wallgraphvector& wg ) : wallgraph(wg) {}

		/* loop over patterns; patterns are either already sorted in reverse order, or the input should be a partial order; can I do sorting during construction from a partial order? start at tail? (topological sorting)
		findoption == 1 -> find at most one match over all patterns (check subset of patterns)
		findoption == 2 -> find at most one match per pattern (check all patterns)
		findoption == 3 -> find number of matches for every pattern (check all patterns)
		*/
		resultslist patternMatch ( std::list<patternvector> allpatterns, const int findoption ); 
		
	private:
		const wallgraphvector& wallgraph;

		void _pruneRegister( patternvector newpattern, patternvector oldpattern, memoize& keepcount );
		uint64_t _patternMatch ( const patternvector pattern, const int findoption, memoize& keepcount );
		bool _checkForWordInLabels( const std::string headpattern, const labelset walllabels );
		void _addToStack ( const bool is_extremum, const uint64_t newpatternlen, const node thisnode, memoize& keepcount, std::stack<node>& nodes_to_visit );
		void _removeMinusOne ( const uint64_t N, memoize& keepcount );
		void _backFill ( const uint64_t N, memoize& keepcount );
};

#endif
