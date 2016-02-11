#include "PatternMatch.hpp"
// #include "DSGRN.hpp", call as: clang++ -std=c++11 -stdlib=libc++ -I ~/GIT/DSGRN/include LIST_OF_FILES


void printMatch( std::list<uint64_t> match ) {
	for ( auto m : match ) {
		std::cout << m << " ";
	}
	std::cout << "\n";
}

bool recursePattern (uint64_t currentwallindex, patternlist pattern, const wallgraphvector& wallgraphptr, const bool boolflag = true ) {

	// memoize should be member of class

	std::size_t key = boost::hash_value(std::pair<currentwallindex,patternlist>);
	
	if ( memoize.count(key) ) {
		; 
	} else if ( pattern.empty() ) {
		if ( boolflag ) {
			return true;
		} else {
			memoize[key] = true;
		}		
	} else {
		auto extremum = pattern.front();
		auto intermediate = pattern.front();
		std::replace(intermediate.begin(),intermediate.end(),'M','I')
		std::replace(intermediate.begin(),intermediate.end(),'m','D')

		auto walllabels = wallgraphptr[ currentwallindex ].walllabels;
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
				memoize[key] = false;
				break; 
			}
		}

		auto outedges = wallgraphptr[ currentwallindex ].outedges;
		auto patterntail = pattern;
		patterntail.pop_front();

		if ( extremum_in_labels ) {
			for ( auto nextwallindex : outedges) {
				if ( recursePattern( nextwallindex, patterntail, wallgraphptr ) ) {
					return true; // do with memosize
				} // else, keep recursing
			}
		}
		if ( intermediate_in_labels ) {
			for ( auto nextwallindex : outedges) {
				if ( recursePattern( nextwallindex, pattern, wallgraphptr ) ) {
					return true; // do with memosize
				} // else, keep recursing
			}
		}
	}
	return false;
}

std::list<uint64_t> match recursePattern_withmatch (uint64_t currentwallindex, patternlist pattern, const wallgraphvector& wallgraphptr, std::list<uint64_t> match) {

	if ( pattern.empty() ) {
		std::cout << "HAVE MATCH!!\n";
		printMatch( match );
		return true;
	} else {
		auto extremum = ( pattern.front() ).extremum;
		auto intermediate = ( pattern.front() ).intermediate;
		auto walllabels = wallgraphptr[ currentwallindex ].walllabels;

		bool extremum_in_labels = ( std::find(walllabels.begin(), walllabels.end(), extremum) != walllabels.end() );
		bool intermediate_in_labels = ( std::find(walllabels.begin(), walllabels.end(), intermediate) != walllabels.end() );

		auto outedges = wallgraphptr[ currentwallindex ].outedges;
		auto patterntail = pattern;
		patterntail.pop_front();

		match.push_back( currentwallindex );

		if ( extremum_in_labels ) {
			for ( auto nextwallindex : outedges) {
				if ( recursePattern_withmatch( nextwallindex, patterntail, wallgraphptr, match ) ) {
					return true;
				} // else, keep recursing
			}
		}
		if ( intermediate_in_labels ) {
			for ( auto nextwallindex : outedges) {
				if ( recursePattern_withmatch( nextwallindex, pattern, wallgraphptr, match ) ) {
					return true;
				} // else, keep recursing
			}
		}
	}

	// std::cout << "fall out\n";
	return false;

}

