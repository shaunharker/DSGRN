/// TestMorseDecomposition.cpp
/// Shaun Harker
/// 2015-05-24

#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>
#include <memory>
#include <string>

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
  Parameter param = pg. parameter ( param_index );
  std::cout << "Chose " << param << "\n";

  // Construct the domain graph
  std::cout << "Construct domain graph.\n";
  DomainGraph dg ( param );

  // Compute the Morse Decomposition
  std::cout << "Construct Morse Decomposition.\n";
  MorseDecomposition md ( dg . digraph () );

  // Construct Morse Graph
  std::cout << "Construct Morse Graph\n";
  MorseGraph mg ( dg, md );

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
