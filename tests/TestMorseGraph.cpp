/// TestMorseDecomposition.cpp
/// Shaun Harker
/// 2015-05-24

#include <iostream>
#include <vector>
#include <cstdlib>
#include <memory>
#include <string>

#include "Parameter/ParameterGraph.h"
#include "Phase/DomainGraph.h"
#include "Dynamics/MorseDecomposition.h"
#include "Dynamics/MorseGraph.h"

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
  std::shared_ptr<DomainGraph> dg ( new DomainGraph );
  dg -> assign ( param );

  // Compute the Morse Decomposition
  std::cout << "Construct Morse Decomposition.\n";
  std::shared_ptr<MorseDecomposition> md ( new MorseDecomposition );
  md -> assign ( dg -> digraph () );

  // Construct Morse Graph
  std::cout << "Construct Morse Graph\n";
  MorseGraph mg;
  mg . assign ( dg, md );

  // Save a graphviz file for the poset
  std::cout << "Save graphviz.\n";
  std::ofstream outfile ( "testmorsegraph.gv" );
  outfile << mg;
  outfile . close ();

  // Print the SHA-256 code
  std::cout << "The SHA-256 code for the Morse Graph is \n  " << mg . SHA256() << "\n";

  // Print the Morse Graph JSON
  std::cout << "JSON:\n";
  std::cout << mg . stringify () << "\n";
  MorseGraph mg2;
  mg2 . parse ( mg . stringify () );
  std::cout << "Reinputting the output:\n";
  std::cout << mg2 . stringify () << "\n";
  return 0;
}
