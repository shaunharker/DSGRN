#include "PatternMatch.hpp"
// #include "DSGRN.hpp", call as: clang++ -std=c++11 -stdlib=libc++ -I ~/GIT/DSGRN/include LIST_OF_FILES


void printMatch( std::list<uint64_t> match ) {
	for ( auto m : match ) {
		std::cout << m << " ";
	}
	std::cout << "\n";
}

bool recursePattern (uint64_t currentwallindex, patternlist pattern, wallgraphvector* wallgraphptr ) {

	if ( pattern.empty() ) {
		return true;
	}

	else {
		auto extremum = ( pattern.front() ).extremum;
		auto intermediate = ( pattern.front() ).intermediate;
		auto walllabels = ( *wallgraphptr )[ currentwallindex ].walllabels;

		bool extremum_in_labels = ( std::find(walllabels.begin(), walllabels.end(), extremum) != walllabels.end() );
		bool intermediate_in_labels = ( std::find(walllabels.begin(), walllabels.end(), intermediate) != walllabels.end() );

		auto outedges = ( *wallgraphptr )[ currentwallindex ].outedges;
		auto patterntail = pattern;
		patterntail.pop_front();

		if ( extremum_in_labels ) {
			for ( auto nextwallindex : outedges) {
				if ( recursePattern( nextwallindex, patterntail, wallgraphptr ) ) {
					return true;
				} // else, keep recursing

			}

		}
		if ( intermediate_in_labels ) {
			for ( auto nextwallindex : outedges) {
				if ( recursePattern( nextwallindex, pattern, wallgraphptr ) ) {
					return true;
				} // else, keep recursing
			}

		}

	}

	return false;

}

bool recursePattern_withmatch (uint64_t currentwallindex, patternlist pattern, wallgraphvector* wallgraphptr, std::list<uint64_t> match) {

	if ( pattern.empty() ) {
		std::cout << "HAVE MATCH!!\n";
		printMatch( match );
		return true;
	}
	else {
		auto extremum = ( pattern.front() ).extremum;
		auto intermediate = ( pattern.front() ).intermediate;
		auto walllabels = ( *wallgraphptr )[ currentwallindex ].walllabels;

		bool extremum_in_labels = ( std::find(walllabels.begin(), walllabels.end(), extremum) != walllabels.end() );
		bool intermediate_in_labels = ( std::find(walllabels.begin(), walllabels.end(), intermediate) != walllabels.end() );

		auto outedges = ( *wallgraphptr )[ currentwallindex ].outedges;
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

