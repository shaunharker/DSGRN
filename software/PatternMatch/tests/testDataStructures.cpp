#include "DataStructures.hpp"
#include <iostream>
#include <vector>

int main() {
	wall wall1 = { {5,6}, {"Muu"} };
	// std::cout << (wall1.outedges == (std::vector<uint64_t>) {5,6}) << "\n";
	wall wall2 = { {2,4,6,7}, {"udd","Mdd","ddd","mdd"}};
	wallgraph wg = {wall1, wall2};

	// use iterators
	std::vector<wall>::iterator it;
	std::vector<uint64_t>::iterator ut;
	std::vector<label>::iterator lt;

	for (it = wg.begin(); it != wg.end(); ++it) {
		// std::cout << (it->outedges == wall1.outedges) << "\n";
		std::cout << "{ ";
		for (ut = (it->outedges).begin(); ut != (it->outedges).end(); ++ut ) {
			std::cout << *ut << " ";
		}
		std::cout << "}\n";
	}
		
	for (it = wg.begin(); it != wg.end(); ++it) {
		std::cout << "{ ";
		for (lt = (it->walllabels).begin(); lt != (it->walllabels).end(); ++lt ) {
			std::cout << *lt << " ";
		}
		std::cout << "}\n";
	}

	// use explicit indexing
	for (int i = 0; i < wg.size(); ++i) {
		// std::cout << (it->outedges == wall1.outedges) << "\n";
		std::cout << "{ ";
		for ( int j = 0; j < (wg[i].outedges).size(); ++j ) {
			std::cout << wg[i].outedges[j] << " ";
		}
		std::cout << "}\n";
	}

	for (int i = 0; i < wg.size(); ++i) {
		std::cout << "{ ";
		for ( int j = 0; j < (wg[i].walllabels).size(); ++j ) {
			std::cout << wg[i].walllabels[j] << " ";
		}
		std::cout << "}\n";
	}	

	// use range-for loop
	for (auto thiswall: wg) {
		std::cout << "{ ";
		for (auto oe: thiswall.outedges) {
			std::cout << oe << " ";
		}
		std::cout << "}\n";
	}	

	for (auto thiswall: wg) {
		std::cout << "{ ";
		for (auto wl: thiswall.walllabels) {
			std::cout << wl << " ";
		}
		std::cout << "}\n";
	}	
}