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
  Network network ( "networks/network2.txt" );

  // Construct a parameter graph
  std::cout << "Construct parameter graph.\n";
  ParameterGraph pg ( network );

  // Fetch a random parameter from the parameter graph
  std::cout << "Choosing parameter.\n";
  uint64_t N = pg . size ();
  uint64_t param_index = 43;
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
  std::cout << md;

  return 0;
}
