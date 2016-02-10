#include "LabelWalls.hpp"

void constructDiffs ( wallStruct* wall, wallgraphvector* wgptr, bool out ) {

	wallStruct other_wall;
	double d;
	bool plustrue;
	bool minustrue;
	std::list<uint64_t> edges;

	if ( out ) {
		edges = wall.outedges;
	}
	else {
		edges = wall.inedges;
	}

	for ( int i = 0; i < DIMENSION; ++i ) {

		plustrue = false;
		minustrue = false;

		for ( auto e : edges ) {
			other_wall = ( *wgptr )[e];
			if ( out ) {
				for ( auto f : other_wall.inedges ) {
					
				}
				
			}
					d = wall->phasespace[ i ] - other_wall.phasespace[ i ]; // this is not the full function
					if ( d > 0 ) {
						plustrue = true;
					}
					else if ( d < 0 ) {
						minustrue = true;
					}
		}

		if ( out ) {
			if ( plustrue && minustrue ) { // no spreading flow should occur
				throw;
			}
			else {
				wall->outsigns[ i ] = minustrue - plustrue;
			}
		}
		else {
			wall->insigns[ i ] = plustrue - minustrue;	// focusing flow OK, value 0
		}
	}
}

std::list<std::string> appendToStringInList ( const std::list<std::string> labels, std::list<std::string> addme ) {

	std::list<std::string>::const_iterator iterator;
	std::list<std::string> newlabels = {};
	std::list<std::string> templabels;
	std::string temp;

	for ( auto a : addme ) {
		templabels = {};
		for ( iterator = labels.begin(); iterator != labels.end(); ++iterator ) { 
			temp = *iterator;
			temp += a;
			templabels.push_back( temp );
		}
		newlabels.splice(newlabels.end(),templabels);
	}

	return newlabels
}

void FTable ( wallStruct* wall ) {

	int os;
	int is;
	std::list<std::string> labels = {""};

	for ( int i = 0; i < DIMENSION; ++i ) {
		os = wall->outsigns[ i ];
		is = wall->insigns[ i ];

		if ( i != wall->var_affected ) { 
			if ( os*is == -1 ) { // no extrema allowed if variable is not affected
				throw;
			}			
			else if ( is == 1 || os == 1 ) {
				labels = appendToStringInList( labels, {"I"} );
			}
			else if ( is == -1 || os == -1) {
				labels = appendToStringInList( labels, {"D"} );
			}
			else {
				labels = appendToStringInList( labels, {"I","D"} );
			}
		}
		else {
			if ( is == -1 && os == 1) { 
				labels = appendToStringInList( labels, {"m"} );
			}			
			else if ( is == 1 && os == -1 ) {
				labels = appendToStringInList( labels, {"M"} );
			}
			else if ( is == 1 && os == 1 ) {
				labels = appendToStringInList( labels, {"I"} );
			}
			else if ( is == -1 && os == -1) {
				labels = appendToStringInList( labels, {"D"} );
			}
			else if ( is == 1 ) {
				labels = appendToStringInList( labels, {"I","M"} );
			}			
			else if ( is == -1 ) {
				labels = appendToStringInList( labels, {"D","m"} );
			}			
			else if ( os == 1 ) {
				labels = appendToStringInList( labels, {"I","m"} );
			}			
			else if ( os == -1 ) {
				labels = appendToStringInList( labels, {"D","M"} );
			}			
			else {
				labels = appendToStringInList( labels, {"I","D","M","m"} );
			}			
		}
	}

	wall->walllabels = labels;
}

void makeWallLabels ( wallgraphvector* wgptr ) {

	for ( auto wall : *wgptr ) {
		constructDiffs ( &wall, wgptr, true )
		constructDiffs ( &wall, wgptr, false)
		FTable ( &wall )

	}
}