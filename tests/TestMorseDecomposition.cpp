/// TestMorseDecomposition.cpp
/// Shaun Harker
/// 2015-05-24

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdint>

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
  ParameterGraph pg ( network );

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
  MorseDecomposition md0;
  MorseDecomposition md ( dg . digraph () );
  MorseDecomposition md2 ( dg . digraph (), md . components () );
  
  // Save a graphviz file for the poset
  std::cout << "Save graphviz.\n";
  std::ofstream outfile ( "testmorsedecomposition.gv" );
  outfile << md;
  outfile . close ();

  return 0;
}
