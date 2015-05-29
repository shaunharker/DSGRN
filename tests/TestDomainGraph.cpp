/// TestDomainGraph.cpp
/// Shaun Harker
/// 2015-05-24

#include <iostream>
#include <vector>
#include <cstdlib>
#include <memory>
#include <string>
#include <fstream>

#include "Parameter/Network.h"
#include "Parameter/ParameterGraph.h"
#include "Phase/DomainGraph.h"

int main ( int argc, char * argv [] ) {
  // Load a network file
  std::cout << "Load network file.\n";
  std::string filename;
  if ( argc < 2 ) filename = "networks/network2.txt";
  else filename = argv[1];
  std::shared_ptr<Network> network ( new Network );
  network -> load ( filename );

  // Construct a parameter graph
  std::cout << "Construct parameter graph.\n";
  ParameterGraph pg;
  pg . assign ( network, "../data/logic/" );

  // Fetch a random parameter from the parameter graph
  std::cout << "Fetch random parameter.\n";
  uint64_t N = pg . size ();
  uint64_t param_index = rand () % N;
  std::shared_ptr<Parameter> param = pg. parameter ( param_index );
  std::cout << "Chose " << *param << "\n";

  // Construct the domain graph
  std::cout << "Construct domain graph.\n";
  DomainGraph dg;
  dg . assign ( param );

  // Save a graphviz file for the domain graph.
  std::cout << "Save graphviz.\n";
  std::ofstream outfile ( "testdomaingraph.gv" );
  outfile << dg;
  outfile . close ();
  return 0;
}
