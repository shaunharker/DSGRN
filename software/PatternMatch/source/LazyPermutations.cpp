#include <iostream>
#include <vector>
#include <list>
#include <String>
#include <algorithm>
#include <iterator>

// input: the transitive reduction of a partial order

typedef std::list<uint64_t> outedges;
typedef std::vector<outedges> graph;

typedef std::list<uint64_t> labels;
typedef std::vector<labels> edgelabels;

bool hamming_distance_is_1(uint64_t n, uint64_t m){
	uint64_t diff = n^m;
	int count = 0; // count accumulates the total bits set 
	while(diff != 0){
	    diff &= (diff-1); // clear the least significant bit set
	    count++;
	    if ( count > 1 ) {
	    	return false;
	    }
	}
	if ( count == 1 ) {
		return true;
	} else {
		return false;
	}
}

graph buildHyperCube ( uint64_t start, uint64_t end ) {
	graph hypercube(end - start);
	for (uint64_t i = start; i < end; ++i ) {
		outedges oe;
		for (uint64_t j = i+1; j < end; ++j ) {
			if ( hamming_distance_is_1( i, j ) ) {
				oe.push_back( j );
			}
		hypercube[ i - start] = oe;
		}
	}
	return hypercube;
}

labels findBasalSet ( graph partial_order ) {
	labels have_in_edges = partial_order[ 0 ];
	have_in_edges.sort();
	labels temp;
	labels alledges(partial_order.size());
	std::iota(std::begin(alledges), std::end(alledges), 0); // range from 0 to partial order size
	labels basal_set; 

	for ( uint64_t i = 1; i < partial_order.size(); ++i ) {
		auto edges = partial_order[ i ];
		edges.sort();
		std::set_union( have_in_edges.begin(), have_in_edges.end(), edges.begin(), edges.end(), std::back_inserter( temp ) );
		have_in_edges = temp;
	}

	std::set_difference(alledges.begin(), alledges.end(), have_in_edges.begin(), have_in_edges.end(), 
                        std::inserter(basal_set, basal_set.begin()));

	return basal_set;
}

std::pair< graph, edgelabels > buildSkeleton ( graph transitive_reduction_of_partial_order ) {
	graph skeleton;
	edgelabels skeleton_labels;
	uint64_t start = 0;
	uint64_t end;

	do {
		auto blist = findBasalSet ( transitive_reduction_of_partial_order );
		end = start + blist.size();
		auto hypercube = buildHyperCube( start, end );
		auto hypercube_labels = makeHyperCubeLabels( hypercube, blist );
		if ( skeleton.size() > 0 ) { // identify first node of new hypercube with last node of previous hypercube
			skeleton.pop_back()
			skeleton_labels.pop_back()
			skeleton.insert( skeleton.end(), hypercube.begin(), hypercube.end() );
			skeleton_labels.insert(skeleton_labels.end(), hypercube_labels.begin(), hypercube_labels.end() );
		} else { 
			skeleton = hypercube;
			skeleton_labels = hypercube_labels;
		}
		start = end-1; // start at last node of hypercube
		getSubGraph( blist, transitive_reduction_of_partial_order ); // reduce partial order in size
	} while ( !transitive_reduction_of_partial_order.empty() )

	return std::make_pair( skeleton, skeleton_labels );
}