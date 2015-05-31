/// TestDomainGraph.cpp
/// Shaun Harker
/// 2015-05-24

#include <iostream>
#include <vector>
#include <cstdint>
#include <memory>
#include <string>
#include <fstream>

#include "DSGRN.h"

int main ( int argc, char * argv [] ) {
  // Load a network file
  std::cout << "Load network file.\n";
  std::string filename;
  if ( argc < 2 ) filename = "networks/network2.txt";
  else filename = argv[1];
  Network network ( filename );

  // Construct a parameter graph
  std::cout << "Construct parameter graph.\n";
  ParameterGraph pg ( network, "../data/logic/" );

  // Fetch a random parameter from the parameter graph
  std::cout << "Fetch random parameter.\n";
  uint64_t N = pg . size ();
  uint64_t param_index = rand () % N;
  Parameter param = pg . parameter ( param_index );
  std::cout << "Chose " << param << "\n";

  // Construct the domain graph
  std::cout << "Construct domain graph.\n";
  DomainGraph dg ( param );

  // Save a graphviz file for the domain graph.
  std::cout << "Save graphviz.\n";
  std::ofstream outfile ( "testdomaingraph.gv" );
  outfile << dg;
  outfile . close ();
  return 0;
}
