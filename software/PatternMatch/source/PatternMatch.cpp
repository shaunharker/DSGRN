#include "PatternMatch.hpp"

PatternMatch::resultslist PatternMatch::patternMatch ( std::list<patternvector> allpatterns, const int findoption ) {

	/* 
	patterns that are intended to be periodic may be overcounted, because we are not guaranteed to return
	to the same place in phase space
	*/

	// make sure findoption has allowable value
	const std::list<int> allowableoptions = {1,2,3};
	if ( std::find( allowableoptions.begin(), allowableoptions.end(), findoption ) == allowableoptions.end() ) {
		std::string errorMessage = std::string("findoption = 1, 2, or 3 only");
		throw std::logic_error(errorMessage);
	}

	PatternMatch::resultslist results;
	patternvector oldpattern = allpatterns.front();
	pathElementMap p;
	PatternMatch::memoize keepcount ( oldpattern.size()+1, p ); // initializes vector of empty maps; keepcount[0] will always be empty; memoization starts at pattern length 1

	for ( auto pattern : allpatterns ) {
		// require nonzero pattern length
		if ( pattern.size() < 1 ) { 
			continue;
		}

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

	// recall keepcount[0] is empty, so memoize is N+1 in size and loop starts at index 1
	pathElementMap p;
	PatternMatch::memoize new_keepcount ( newpattern.size()+1, p );
	for ( int j = 1; j < tailmatch+1; ++j ) { 
		new_keepcount[ j ] = keepcount[ j ];
	}
	keepcount = new_keepcount;
}

uint64_t PatternMatch::_patternMatch ( const patternvector pattern, const int findoption, PatternMatch::memoize& keepcount ) {

	// pattern length required for indexing throughout function
	uint64_t N = pattern.size();

	// initialize stack with all walls
	std::stack< PatternMatch::node > nodes_to_visit; 
	for ( uint64_t i = 0; i < wallgraph.size(); ++i ) {
		nodes_to_visit.push( std::make_pair( i, N ) );
	}

	// construct intermediate labels for pattern
	patternvector temp = pattern;
	for ( auto& t : temp ) {
		std::replace(t.begin(),t.end(),'M','I');
		std::replace(t.begin(),t.end(),'m','D');		
	}
	const patternvector intermediates = temp;

	// depth first search
	while ( !nodes_to_visit.empty() ) {

		auto thisnode = nodes_to_visit.top();
		nodes_to_visit.pop();

		// convenience variables
		auto wall = thisnode.first;
		auto patternlen = thisnode.second;

		// // DEBUG
		// std::cout << "Wall: " << wall << ", Pattern length: " << patternlen << "\n";
		// // END DEBUG

		// check if wall matches pattern head
		auto walllabels = wallgraph[ wall ].labels;
		bool extremum_in_labels = _checkForWordInLabels( pattern[ N - patternlen ], walllabels );
		bool intermediate_in_labels = _checkForWordInLabels( intermediates[ N - patternlen ], walllabels );

		// access memoization structure keepcount for thisnode
		auto keyT = std::make_pair( wall, true );
		auto keyF = std::make_pair( wall, false );

		// ensure that thisnode has keys in keepcount; needed for _addToStack (could rewrite if needed)
		// assign 0 for no paths, -1 for number of paths to be determined
		if ( !extremum_in_labels && !intermediate_in_labels ) {
			( keepcount[ patternlen ] )[ keyT ] = 0;
			( keepcount[ patternlen ] )[ keyF ] = 0;
			continue; 
		} else if ( !intermediate_in_labels || patternlen == N ) { // patternlen == N -> don't search intermediate labels at top nodes
			( keepcount[ patternlen ] )[ keyF ] = 0;
		} else if ( !extremum_in_labels ) {
			( keepcount[ patternlen ] )[ keyT ] = 0;
		}

		if ( ( keepcount[ patternlen ] ).count( keyT ) == 0 ) {
			( keepcount[ patternlen ] )[ keyT ] = -1;
		}
		if ( ( keepcount[ patternlen ] ).count( keyF ) == 0 ) {
			( keepcount[ patternlen ] )[ keyF ] = -1;
		} 

		// now check for new nodes to be added to the stack and update the memoization structure
		// can only combine with above if statement if _addToStack is rewritten
		if ( ( keepcount[ patternlen ] )[ keyF ] == -1 ) { // intermediate
		// // DEBUG
		// std::cout << "Intermediate: Wall: " << keyF.first << ", is_extremum: " << keyF.second << "\n";
		// // END DEBUG
			_addToStack( false, patternlen, thisnode, keepcount, nodes_to_visit );
		}

		if ( ( keepcount[ patternlen ] )[ keyT ] == -1 ) { // extremum
			if ( patternlen > 1 ) {
				_addToStack( true, patternlen-1, thisnode, keepcount, nodes_to_visit );
			// else if patternlen == 1 and extremum_in_labels, we have reached the final leaf in a path
			} else if ( findoption == 1 ) { 
				return 1; // we only seek one pattern match over all patterns
			} else if ( findoption == 2 ) {
				_removeMinusOne ( N, keepcount ); // keep only completed portions of memoization structure for the next pattern
				return 1;
			} else {
				( keepcount[ patternlen ] )[ keyT ] = 1; // we seek all matches; keep going
			}
		}
	}

	// DEBUG
	std::cout << "Made it through while loop.\n";
	// END DEBUG

	// backfill memoization structure -> want this even for findoptions 1 and 2 to save computation
	_backFill ( N, keepcount );

	// DEBUG
	std::cout << "Made it past backfill.\n";
	for ( int q =0; q < N+1; ++q ) {
		std::cout << "Pattern length: " << q << "\n";
		for ( auto kvp : keepcount[ q ] ){	
			std::cout << "Wall: " << kvp.first.first << ", is_extremum: " << kvp.first.second << ", Count: " << kvp.second << "\n";
		}			
	}
	// END DEBUG


	if ( findoption < 3 ) {
		return 0;
	} else {
		// in findoption == 3, we count the total matches for this pattern from the cumsums at the full pattern length 
		uint64_t count = 0;
		for ( auto kvp : keepcount[ N ] ){
			count += kvp.second;
		}
		return count;
	}
}

bool PatternMatch::_checkForWordInLabels( const std::string headpattern, const labelset walllabels ) {
	// match all chars
	for ( int i = 0; i < walllabels.size(); ++i ) {
		if ( walllabels[ i ].count( headpattern[ i ] ) == 0 ) {
			return false;	
		}
	}
	return true;	
}

void PatternMatch::_addToStack ( const bool is_extremum, const uint64_t newpatternlen, const PatternMatch::node thisnode, PatternMatch::memoize& keepcount, std::stack< PatternMatch::node >& nodes_to_visit ) {

	bool assign = true;
	uint64_t numpaths = 0;
	auto outedges = wallgraph[ thisnode.first ].outedges;

	// add new walls to stack and memoization structure
	for ( auto nextwall : outedges) {
		auto keyT = std::make_pair( nextwall, true );
		auto keyF = std::make_pair( nextwall, false );
		auto inthereT = ( keepcount[ newpatternlen ] ).count( keyT );
		auto inthereF = ( keepcount[ newpatternlen ] ).count( keyF );
		// we know that either both keys or neither key have been assigned to keepcount
		// this is ensured because thisnode has both keys initialized before either adds to the stack
		// rewrite the if statement below if the if statements in _patternMatch are combined
		if ( !inthereT && !inthereF ) {
			assign = false;
			nodes_to_visit.push( std::make_pair( nextwall, newpatternlen ) );			
		} else {
			auto pathsT = ( keepcount[ newpatternlen ] )[ keyT ];
			auto pathsF = ( keepcount[ newpatternlen ] )[ keyF ];
			// if either pathsT or pathsF have counts to be determined, can't update thisnode with number of paths
			if ( ( pathsT == -1 ) || ( pathsF == -1 ) ) {
				assign = false;
			} else if ( assign ) {
				numpaths += ( pathsF + pathsT );
			}
		}
	}

	// update cumsums -> if all children previously traversed, add together number of paths
	if ( assign ) {
		auto key = std::make_pair( thisnode.first, is_extremum );
		( keepcount[ thisnode.second ] )[ key ] = numpaths;
	}
}

void PatternMatch::_removeMinusOne ( const uint64_t N, memoize& keepcount ) {
	// if the number of paths has not been determined, remove the key value pair from the map assigned to pattern length i
	for ( uint64_t i = 0; i < N+1; ++i ) {
		for ( auto kvp : keepcount[ i ] ) {
			if ( kvp.second == -1 ) {
				keepcount[ i ].erase ( kvp.first );
			}
		}
	}
}

void PatternMatch::_backFill ( const uint64_t N, memoize& keepcount ) {

	// complete memoization structure iteratively, starting with the leaves of the paths

	PatternMatch::_backFillIntermediate ( 1, keepcount);

	for ( auto kvp : keepcount[1] ) {
		// // DEBUG
		// std::cout << "Wall: " << (kvp.first).first << ", is_extremum: " << (kvp.first).second << ", Count value: " << kvp.second << "\n";
		// // END DEBUG
		if ( ( kvp.first ).second && ( kvp.second != 0 ) && ( kvp.second != 1 ) ) {
			std::string errorMessage = std::string("Not all extremal leaves assigned 0 or 1.");
			throw std::logic_error(errorMessage); // leaves should all be assigned to either 0 or 1 at this point
		}
	}

	// DEBUG
	std::cout << "All leaves assigned.\n";
	// END DEBUG


	for ( uint64_t i = 2; i < N+1; ++i ) {
		
		// starting with pattern length 2, fill in all path counts for pattern length i

		// first, calculate all extrema counts using i-1 pattern length -- these are guaranteed to exist
		for ( auto& kvp : keepcount[ i ] ) {
			uint64_t accum = 0;
			auto wall = ( kvp.first ).first;
			auto is_extremum = ( kvp.first ).second;
			auto outedges = wallgraph[ wall ].outedges;
			if ( is_extremum ) {
				for ( auto oe : outedges ) {
					auto keyT = std::make_pair( oe, true );
					auto keyF = std::make_pair( oe, false );
					accum += ( ( keepcount[ i-1 ] )[ keyT ] + ( keepcount[ i-1 ] )[ keyF ] );
				}
				kvp.second = accum;
				// // DEBUG
				// std::cout << "Wall: " << wall << ", Count: " << accum << "\n";
				// // END DEBUG
			} 
		}
		// second, calculate all intermediate counts using i pattern length -- not guaranteed to exist yet, must proceed iteratively
		PatternMatch::_backFillIntermediate ( i, keepcount);
	}
}

void PatternMatch::_backFillIntermediate ( const uint64_t i, memoize& keepcount) {
	bool done = false;
	while ( !done ) {
		bool anyminusone = false;
		for ( auto& kvp : keepcount[ i ] ) {
			auto wall = ( kvp.first ).first;
			auto is_extremum = ( kvp.first ).second;

			if ( is_extremum || kvp.second > -1 ) {
				continue;
			}

			// // DEBUG
			// std::cout << "Wall: " << wall << ", is_extremum: " << is_extremum << ", Count value: " << kvp.second << "\n";
			// // END DEBUG

			auto outedges = wallgraph[ wall ].outedges;
			uint64_t numpaths = 0;
			bool assign = true;
			for ( auto oe : outedges ) {
				auto keyT = std::make_pair( oe, true);
				auto keyF = std::make_pair( oe, false);
				auto oe_count_T = ( keepcount[ i ] )[ keyT ];
				auto oe_count_F = ( keepcount[ i ] )[ keyF ];
				// // DEBUG
				// std::cout << "Wall: " << oe << ", true count: " << oe_count_T << ", false count: " << oe_count_F << "\n";
				// // END DEBUG

				if ( oe_count_T == -1 || oe_count_F == -1 ) {
					assign = false;
					anyminusone = true;
					break;
				} else {
					numpaths += ( oe_count_T + oe_count_F );
				}
			}
			if ( assign ) {
				kvp.second = numpaths;
			}			
		}
		if ( !anyminusone ) {
			done = true;
		}
	}
}


// // Below: previous version written recursively without memoization

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

