/// TestParameterGraph.cpp
/// Shaun Harker
/// 2015-05-24

#include <iostream>
#include <vector>
#include <cstdlib>

#include "Parameter/ParameterGraph.h"

int main ( int argc, char * argv [] ) {
  std::string filename;
  if ( argc < 2 ) filename = "networks/network2.txt";
  else filename = argv[1];
  std::shared_ptr<Network> network ( new Network );
  network -> load ( filename );
  ParameterGraph pg;
  pg . assign ( network, "../data/logic/" );
  std::cout << pg << "\n";
  uint64_t N = pg . size ();
  for ( uint64_t i = 0; i < N; ++ i ) {
    std::shared_ptr<Parameter> p = pg . parameter ( i );
    std::shared_ptr<Parameter> q = pg . parameter ( 0 );
    std::string s = p -> stringify ();
    q -> parse ( s );
    std::cout << * q << "\n";
  }
  return 0;
}
