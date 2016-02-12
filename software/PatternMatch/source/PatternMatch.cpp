#include "PatternMatch.hpp"

bool patternMatch (uint64_t startwall, patternlist startpattern, const wallgraphvector& wallgraphptr ) {

	typedef std::pair<uint64_t,patternlist> wallpair;
	std::deque<wallpair> nodes_to_visit;

	wallpair root = std::make_pair(startwall,startpattern);
	nodes_to_visit.push_front(root);

	while ( !(nodes_to_visit.empty()) ) {
		// std::cout << "New while: \n";
		// for (auto p : nodes_to_visit) {
		// 	std::cout << p.first << "\n";
		// }
		// std::cout << "\n";

		wallpair currentpair = nodes_to_visit.front();
		nodes_to_visit.pop_front();
		auto wall = currentpair.first;
		auto pattern = currentpair.second;

		if ( pattern.empty() ) {
			return true;
		}

		auto extremum = pattern.front();
		auto intermediate = pattern.front();
		std::replace(intermediate.begin(),intermediate.end(),'M','I');
		std::replace(intermediate.begin(),intermediate.end(),'m','D');
		// std::cout << "extremum: " << extremum << ", intermediate: " << intermediate << "\n"; 

		auto walllabels = wallgraphptr[ wall ].labels;
		bool extremum_in_labels = true;
		bool intermediate_in_labels = true;

		for ( int i = 0; i < walllabels.size(); ++i ) {
			if ( walllabels[ i ].count( extremum[ i ] ) == 0) {
				extremum_in_labels = false;				
			}
			if ( walllabels[ i ].count( intermediate[ i ] ) == 0) {
				intermediate_in_labels = false;				
			}
			if ( !extremum_in_labels && !intermediate_in_labels ) {
				break; 
			}
		}

		if ( !extremum_in_labels && !intermediate_in_labels ) {
			continue; 
		}

		auto outedges = wallgraphptr[ wall ].outedges;
		auto patterntail = pattern;
		patterntail.pop_front();

		if ( intermediate_in_labels ) {
			for ( auto nextwallindex : outedges) {
				nodes_to_visit.push_front( std::make_pair( nextwallindex, pattern ) );
			} 
		}
		if ( extremum_in_labels ) {
			for ( auto nextwallindex : outedges) {
				nodes_to_visit.push_front( std::make_pair( nextwallindex, patterntail ) );
			} 
		}
	}
	return false;
}

// boost::unordered_map memoize = {};

// bool recursePattern (uint64_t currentwallindex, patternlist pattern, const wallgraphvector& wallgraphptr, const bool boolflag = true ) {

// 	// memoize should be member of class
// 	// wallgraph should be member of class

// 	static bool foundmatch = false;
// 	std::pair<uint64_t,patternlist> key (currentwallindex,pattern);
	
// 	if ( !(memoize.count(key)) ) {
// 		if ( pattern.empty() ) {
// 			if ( boolflag ) {
// 				return true;
// 			} else {
// 				foundmatch = true;
// 				memoize[key] = true;
// 			}		
// 		} else {
// 			auto extremum = pattern.front();
// 			auto intermediate = pattern.front();
// 			std::replace(intermediate.begin(),intermediate.end(),'M','I')
// 			std::replace(intermediate.begin(),intermediate.end(),'m','D')

// 			auto walllabels = wallgraphptr[ currentwallindex ].walllabels;
// 			bool extremum_in_labels = true;
// 			bool intermediate_in_labels = true;

// 			for ( int i = 0; i < walllabels.size(); ++i ) {
// 				if ( walllabels[ i ].count( extremum[ i ] ) == 0) {
// 					extremum_in_labels = false;				
// 				}
// 				if ( walllabels[ i ].count( intermediate[ i ] ) == 0) {
// 					intermediate_in_labels = false;				
// 				}
// 				if ( !extremum_in_labels && !intermediate_in_labels ) {
// 					memoize[key] = false;
// 					break; 
// 				}
// 			}

// 			auto outedges = wallgraphptr[ currentwallindex ].outedges;
// 			auto patterntail = pattern;
// 			patterntail.pop_front();

// 			if ( extremum_in_labels ) {
// 				for ( auto nextwallindex : outedges) {
// 					if ( recursePattern( nextwallindex, patterntail, wallgraphptr ) ) {
// 						if ( boolflag ) {
// 							return true;
// 						} else {
// 							foundmatch = true;
// 							memoize[key] = true;
// 						}		
// 					} // else, keep recursing
// 				}
// 			}
// 			if ( intermediate_in_labels ) {
// 				for ( auto nextwallindex : outedges) {
// 					if ( recursePattern( nextwallindex, pattern, wallgraphptr ) ) {
// 						if ( boolflag ) {
// 							return true;
// 						} else {
// 							foundmatch = true;
// 							memoize[key] = true;
// 						}		
// 					} // else, keep recursing
// 				}
// 			}
// 		}
// 		return foundmatch;
// 	}
// }

// void printMatch( std::list<uint64_t> match ) {
// 	for ( auto m : match ) {
// 		std::cout << m << " ";
// 	}
// 	std::cout << "\n";
// }

// std::list<uint64_t> match recursePattern_withmatch (uint64_t currentwallindex, patternlist pattern, const wallgraphvector& wallgraphptr, std::list<uint64_t> match) {

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

