#include "DataStructures.hpp"

wallgraphvector WallGraph;

void constructDiffs ( wallStruct* wall, wallgraphvector* wgptr, bool out ) {

	wallStruct other_wall;
	double d;
	bool plustrue;
	bool minustrue;
	auto edges;

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
			d = wall->phasespace[ i ] - other_wall.phasespace[ i ];
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
			wall->insigns[ i ] = plustrue - minustrue;		
		}
	}
}

void FTable ( wallStruct* wall ) {

	int os;
	int is;
	std::list<std::string> labels = {""};

	for ( int i = 0; i < DIMENSION; ++i ) {
		os = wall->outsigns[ i ];
		is = wall->insigns[ i ];

		if ( i != wall->var_affected ) { // no extrema allowed if variable is not affected
			if ( os*is == -1 ) {
				throw;
			}
			else if ( is == 1 || os == 1 ) {
				for ( auto label : labels ) { // will this work? do I need a pointer?
					label += "I"
				}
			}
			else if ( is == -1 || os == -1) {
				for ( auto label : labels ) { // will this work? do I need a pointer?
					label += "D"
				}
			}
			else {
				// make new labels for both I and D
				;
			}
		}
	}
}

void makeWallLabels ( wallgraphvector* wgptr ) {

	for ( auto wall : *wgptr ) {
		constructDiffs ( &wall, wgptr, true )
		constructDiffs ( &wall, wgptr, false)

	}
}