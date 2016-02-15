#include "PatternMatch.hpp"

resultslist PatternMatch::patternMatch ( std::list<patternvector> allpatterns ) {

	memoize keepcount; 
	resultslist results;
	patternvector oldpattern = allpatterns.front();

	for ( auto pattern : allpatterns ) {
		_pruneRegister( pattern, oldpattern, keepcount );
		nummatches = _patternMatch( pattern, keepcount );

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

void PatternMatch::_pruneRegister( patternvector newpattern, patternvector oldpattern, memoize& keepcount ) {
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
		keepcount.clear();
	} else {
		for ( auto kvp : keepcount ) {
			if ( ( kvp.first ).second > tailmatch ) {
				keepcount.erase(kvp.first);
			}
		}
	}
}

uint64_t PatternMatch::_patternMatch ( const patternvector pattern, memoize& keepcount ) {

	// construct intermediate labels
	std::vector<std::string> temp = pattern;
	for ( auto& t : temp ) {
		std::replace(t.begin(),t.end(),'M','I');
		std::replace(t.begin(),t.end(),'m','D');		
	}
	const std::vector<std::string> intermediates = temp;

	// initialize the stack and store top level nodes
	uint64_t N = pattern.size();
	if ( N < 1 ) { // require nonzero length pattern for extremum_in_labels if block
		return 0;
	}
	nodes_to_visit = _initializeStack ( N, pattern.front() ); // note initialize is recalculated in the while loop, is there a better way to do this?
	if ( N == 1 ) { // result for trivial pattern
		return nodes_to_visit.size();
	}
	const auto original_keys = nodes_to_visit; // only used if findoption==3

	// depth first search
	while ( !nodes_to_visit.empty() ) {

		auto thisnode = nodes_to_visit.front()
		nodes_to_visit.pop_front();

		auto wall = thisnode.first;
		auto patternlen = thisnode.second;
		auto walllabels = wallgraph[ wall ].labels;
		bool extremum_in_labels = _checkForWordInLabels( pattern[ N - patternlen ], walllabels );
		bool intermediate_in_labels = _checkForWordInLabels( intermediates[ N - patternlen ], walllabels );

		if ( !extremum_in_labels && !intermediate_in_labels ) {
			keepcount[ thisnode ] = 0;
			continue; 
		}
		if intermediate_in_labels {
			_addToStack( patternlen, thisnode );
		}
		if extremum_in_labels { // implicitly, patternlen > 0
			if ( patternlen > 1 ) {
				_addToStack( patternlen-1, thisnode );
			// else if patternlen == 1 and extremum_in_labels, we have reached the final leaf in a path
			} else if ( findoption < 3 ) { 
				return 1;
			} else {
				keepcount[ thisnode ] = 1; 
			}			
		}
	}

	if ( findoption < 3 ) {
		return 0;
	} else {
		uint64_t count = 0;
		for ( auto key : original_keys ) {
			count += keepcount[ key ];
		}
		return count;
	}
}

std::stack<node> PatternMatch::_initializeStack ( const uint64_t N, const std::string extremum ) {

	std::stack<node> nodes_to_visit;

	for ( uint64_t i = 0; i < wallgraph.size(); ++i ) {
		if ( _checkForWordInLabels( extremum, wallgraph[ i ].labels ) ) {
			nodes_to_visit.push_front( std::make_pair( i, N ) );
		}
	}
	return nodes_to_visit;
}

bool PatternMatch::_checkForWordInLabels( const std::string word, const std::vector<std::set<char>> labels) {
	bool inthere = true;
	for ( int i = 0; i < labels.size(); ++i ) {
		if ( labels[ i ].count( word[ i ] ) == 0 ) {
			inthere = false;	
			break;			
		}
	}
	return inthere;	
}

void PatternMatch::_addToStack ( uint64_t patternlen, node thisnode ) {

	bool assign = true;
	uint64_t numpaths = 0;
	auto outedges = wallgraph[ thisnode.first ].outedges;
	node new_node;

	for ( auto nextwall : outedges) {
		new_node = std::make_pair( nextwall, patternlen );
		if ( !keepcount.count( new_node ) ) {
			assign = false;
			nodes_to_visit.push_front( new_node );
		} else if ( assign ) {
			numpaths += keepcount[ new_node ];
		} 
	} 
	if ( assign ) {
		// if all children previously traversed, add together number of paths
		keepcount[ thisnode ] = numpaths; 
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

