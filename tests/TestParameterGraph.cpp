/// TestParameterGraph.cpp
/// Shaun Harker
/// 2015-05-24

#include "common.h"
#include "DSGRN.h"

int main ( int argc, char * argv [] ) {
  try {
    ParameterGraph pg0;
    std::string filename;
    if ( argc < 2 ) filename = "networks/network2.txt";
    else filename = argv[1];
    Network network ( filename );
    ParameterGraph pg ( network );
    std::cout << pg << "\n";
    std::cout << pg . network () << "\n";
    uint64_t N = pg . size ();
    for ( uint64_t i = 0; i < N; ++ i ) {
      Parameter p = pg . parameter ( i );
      Parameter q = pg . parameter ( 0 );
      std::string s = p . stringify ();
      q . parse ( s );
      std::cout << q << "\n";
    }

    if ( pg . size () != 120 ) throw std::runtime_error ( "ParameterGraph::size bug");
    if ( pg . fixedordersize () != 60 ) throw std::runtime_error ( "ParameterGraph::fixedordersize bug");
    if ( pg . reorderings () != 2 ) throw std::runtime_error ( "ParameterGraph::reorderings bug");

    std::cout << network.graphviz() << "\n\n";

    Parameter z = pg . parameter ( 0 );

    // Test ParameterGraph::index
    for ( uint64_t i = 0; i < N; ++i ) {
      uint64_t j = pg . index ( pg . parameter(i) );
      if ( i != j ) {
        throw std::runtime_error("ParameterGraph::index bug");
      }
    }

    // Test ParameterGraph::adjacencies
    std::cout << "Testing adjacencies.\n";
    auto adj = pg . adjacencies ( 57 );
    for ( auto j : adj ) {
      std::cout << pg. parameter (j) . stringify () << "\n";
    }

    // Test loading an unsupported network (should throw)
    try {
      Network net ( "networks/network4.txt" );
      ParameterGraph pg1 ( net );
    } catch ( ... ) {}

    // Test serialization
    boost::archive::text_oarchive oa(std::cout);
    oa << pg;

  } catch ( std::exception & e ) {
    std::cout << e . what () << "\n";
    return 1;
  }
  return 0;
}
