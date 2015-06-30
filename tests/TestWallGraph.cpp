/// TestWallGraph.cpp
/// Shaun Harker
/// 2015-06-30

#include <iostream>
#include <vector>
#include <cstdint>
#include <memory>
#include <string>
#include <fstream>

#include "DSGRN.h"

int main ( int argc, char * argv [] ) {
  try { 
    // Load a network file
    std::cout << "Load network file.\n";
    std::string filename;
    if ( argc < 2 ) filename = "networks/network2.txt";
    else filename = argv[1];
    Network network ( filename );

    // Construct a parameter graph
    std::cout << "Construct parameter graph.\n";
    ParameterGraph pg ( network );

    // Fetch a random parameter from the parameter graph
    std::cout << "Fetch random parameter.\n";
    uint64_t N = pg . size ();
    uint64_t param_index = rand () % N;
    Parameter param = pg . parameter ( param_index );
    std::cout << "Chose " << param << "\n";

    // Construct the domain graph
    std::cout << "Construct wall graph.\n";
    WallGraph wg ( param );

    // Default constructor
    WallGraph wg0;

    // Test <<
    std::cout << wg;
    boost::archive::text_oarchive oa(std::cout);
    oa << wg;

    // Note:
    //   WallGraph::digraph, WallGraph::annotate tested in TestMorseGraph.cpp
  } catch ( std::exception& e ) {
    std::cout << e . what () << "\n";
    return 1;
  }
  return 0;
}
