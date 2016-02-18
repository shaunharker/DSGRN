#include "PatternMatch.hpp"

PatternMatch::resultslist PatternMatch::patternMatch ( std::list<patternvector> allpatterns, const int findoption ) {

	/* 
	patterns that are intended to be periodic may be overcounted, because we are not guaranteed to return
	to the same place in phase space
	*/

	// make sure findoption has allowable value
	const std::list<int> allowableoptions = {1,2,3};
	if ( std::find( allowableoptions.begin(), allowableoptions.end(), findoption ) == allowableoptions.end() ) {
		throw;
	}

	PatternMatch::resultslist results;
	patternvector oldpattern = allpatterns.front();
	PatternMatch::memoize keepcount ( oldpattern.size()+1, std::make_pair({},{}) ); // initializes vector of pairs of empty vectors; keepcount[0] will always be a pair of empties; memoization starts at pattern length 1

	for ( auto pattern : allpatterns ) {
		// keep keys in keepcount that are going to be searched again
		_pruneRegister( pattern, oldpattern, keepcount );
		// perform the pattern match
		uint64_t nummatches = _patternMatch( pattern, findoption, keepcount );

		if ( nummatches > 0 ) {
			results.push_back( std::make_pair(pattern, nummatches) );
			if ( findoption == 1 ) {
				return results;
			}
		}
		oldpattern = pattern;
	}
	return results;
}

void PatternMatch::_pruneRegister( patternvector newpattern, patternvector oldpattern, PatternMatch::memoize& keepcount ) {
	std::reverse(newpattern.begin(),newpattern.end());
	std::reverse(oldpattern.begin(),oldpattern.end());
	uint64_t tailmatch = 0;

	for ( uint64_t i = 0; i < std::min(newpattern.size(),oldpattern.size()); ++i ) {
		if ( newpattern[ i ] == oldpattern[ i ] ) {
			tailmatch += 1;
		} else {
			break;
		}
	}

	if ( tailmatch == 0 ) {
		keepcount.assign ( newpattern.size()+1, std::make_pair({},{}) );
	} else {
		PatternMatch::memoize new_keepcount ( newpattern.size()+1, std::make_pair({},{}) );
		// recall keepcount[0] is empty; matching starts at pattern length 1
		for ( int j = 1; j < tailmatch+1; ++j ) { 
			new_keepcount[ j ] = keepcount[ j ];
		}
		keepcount = new_keepcount;
	}
}

uint64_t PatternMatch::_patternMatch ( const patternvector pattern, const int findoption, PatternMatch::memoize& keepcount ) {

	// require nonzero length pattern 
	uint64_t N = pattern.size();
	if ( N < 1 ) { 
		return 0;
	}

	// construct intermediate labels
	std::vector<std::string> temp = pattern;
	for ( auto& t : temp ) {
		std::replace(t.begin(),t.end(),'M','I');
		std::replace(t.begin(),t.end(),'m','D');		
	}
	const std::vector<std::string> intermediates = temp;

	// initialize stack
	std::stack< PatternMatch::node > nodes_to_visit; 
	for ( uint64_t i = 0; i < wallgraph.size(); ++i ) {
		nodes_to_visit.push( std::make_pair( i, N ) );
	}

	// depth first search
	while ( !nodes_to_visit.empty() ) {

		auto thisnode = nodes_to_visit.top();
		nodes_to_visit.pop();

		auto wall = thisnode.first;
		auto patternlen = thisnode.second;
		auto walllabels = wallgraph[ wall ].labels;
		bool extremum_in_labels = _checkForWordInLabels( pattern[ N - patternlen ], walllabels );
		bool intermediate_in_labels = _checkForWordInLabels( intermediates[ N - patternlen ], walllabels );

		if ( !extremum_in_labels && !intermediate_in_labels ) {
			( keepcount[ patternlen ].first ).push_back( wall );
			( keepcount[ patternlen ].second ).push_back( 0 );
			continue; 
		}

		if ( intermediate_in_labels && ( patternlen < N ) ) { // patternlen < N -> don't search intermediate labels at top nodes
			_addToStack( patternlen, thisnode, keepcount, nodes_to_visit );
		}

		if ( extremum_in_labels ) { 
			if ( patternlen > 1 ) {
				_addToStack( patternlen-1, thisnode, keepcount, nodes_to_visit );
			// else if patternlen == 1 and extremum_in_labels, we have reached the final leaf in a path
			} else if ( findoption < 3 ) { 
				// FIXME: memoization structure incomplete for findoption 2; will have to remove -1 entries, because these represent unfinished nodes in the stack and they won't be in the stack in a new search
				// findoption 1 can just have return statement because the for loop will break anyway
				return 1;
			} else {
				keepcount[ thisnode ] = 1; 
			}			
		}
	}

	// backfill memo structure -> want this even for findoptions 1 and 2 to save computation

	for ( auto i : keepcount[1].second ) {
		if ( i != 0 ) && ( i != 1 ) {
			throw; // leaves should all be assigned
		}
	}

	for ( uint64_t i = 2; i < N; ++i ) {
		auto walls =  keepcount[ i ].first;
	}

	if ( findoption < 3 ) {
		return 0;
	} else {
		uint64_t count = 0;
		for ( auto n : keepcount[ N-1 ].second ){
			count += n;
		}
		return count;
	}
}

std::stack<PatternMatch::node> PatternMatch::_initializeStack ( const uint64_t N, const std::string extremum ) {

	std::stack<PatternMatch::node> nodes_to_visit;

	for ( uint64_t i = 0; i < wallgraph.size(); ++i ) {
		if ( _checkForWordInLabels( extremum, wallgraph[ i ].labels ) ) {
			nodes_to_visit.push( std::make_pair( i, N ) );
		}
	}
	return nodes_to_visit;
}

bool PatternMatch::_checkForWordInLabels( const std::string word, const std::vector<std::set<char>> labels) {
	// match all
	for ( int i = 0; i < labels.size(); ++i ) {
		if ( labels[ i ].count( word[ i ] ) == 0 ) {
			return false;	
		}
	}
	return true;	
}

void PatternMatch::_addToStack ( const uint64_t newpatternlen, const PatternMatch::node thisnode, PatternMatch::memoize& keepcount, std::stack< PatternMatch::node >& nodes_to_visit ) {

	bool assign = true;
	uint64_t numpaths = 0;
	auto outedges = wallgraph[ thisnode.first ].outedges;
	auto knownwalls = keepcount[ newpatternlen ].first; 

	// add new walls to stack and memoization structure
	for ( auto nextwall : outedges) {
		int i;
		for ( i = 0; i < knownwalls.size(), ++i ) {
			if ( nextwall == knownwalls[ i ] ) {
				break;
			} 
		}
		if ( i == knownwalls.size() ) {
			assign = false;
			nodes_to_visit.push( std::make_pair( nextwall, newpatternlen ) );
			( keepcount[ newpatternlen ].first ).push_back( nextwall );
			( keepcount[ newpatternlen ].second ).push_back( -1 );			
		} else if ( ( keepcount[ newpatternlen ].second )[ i ] < 0 ) {
			assign = false;
		} else if ( assign ) {
			numpaths += ( keepcount[ newpatternlen ].second )[ i ];
		}
	} 

	// update cumsums -> if all children previously traversed, add together number of paths
	if ( assign ) {
		auto walls = keepcount[ thisnode.second ].first;
		for ( int i = 0; i < walls.size(); ++i ) {
			if ( thisnode.first == walls[ i ] ) {
				( keepcount[ thisnode.second ].second )[ i ] = numpaths;
				break;
			}
		}
	}
}



// void printMatch( std::list<uint64_t> match ) {
// 	for ( auto m : match ) {
// 		std::cout << m << " ";
// 	}
// 	std::cout << "\n";
// }

// std::list<uint64_t> match recursePattern_withmatch (uint64_t currentwallindex, patternvector pattern, const wallgraphvector& wallgraphptr, std::list<uint64_t> match) {

// 	if ( pattern.empty() ) {
// 		std::cout << "HAVE MATCH!!\n";
// 		printMatch( match );
// 		return true;
// 	} else {
// 		auto extremum = ( pattern.front() ).extremum;
// 		auto intermediate = ( pattern.front() ).intermediate;
// 		auto walllabels = wallgraphptr[ currentwallindex ].walllabels;

// 		bool extremum_in_labels = ( std::find(walllabels.begin(), walllabels.end(), extremum) != walllabels.end() );
// 		bool intermediate_in_labels = ( std::find(walllabels.begin(), walllabels.end(), intermediate) != walllabels.end() );

// 		auto outedges = wallgraphptr[ currentwallindex ].outedges;
// 		auto patterntail = pattern;
// 		patterntail.pop_front();

// 		match.push_back( currentwallindex );

// 		if ( extremum_in_labels ) {
// 			for ( auto nextwallindex : outedges) {
// 				if ( recursePattern_withmatch( nextwallindex, patterntail, wallgraphptr, match ) ) {
// 					return true;
// 				} // else, keep recursing
// 			}
// 		}
// 		if ( intermediate_in_labels ) {
// 			for ( auto nextwallindex : outedges) {
// 				if ( recursePattern_withmatch( nextwallindex, pattern, wallgraphptr, match ) ) {
// 					return true;
// 				} // else, keep recursing
// 			}
// 		}
// 	}

// 	// std::cout << "fall out\n";
// 	return false;

// }

