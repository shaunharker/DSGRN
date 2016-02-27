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
    // for ( uint64_t i = 0; i < N; ++ i ) {
    //   Parameter p = pg . parameter ( i );
    //   Parameter q = pg . parameter ( 0 );
    //   std::string s = p . stringify ();
    //   q . parse ( s );
      // std::cout << q << "\n";
    // }
    // if ( pg . size () != 120 ) throw std::runtime_error ( "ParameterGraph::size bug");
    // if ( pg . fixedordersize () != 60 ) throw std::runtime_error ( "ParameterGraph::fixedordersize bug");
    // if ( pg . reorderings () != 2 ) throw std::runtime_error ( "ParameterGraph::reorderings bug");


    std::cout << network.graphviz() << "\n\n";

    // Missing Features (currently they throw):
    Parameter z = pg . parameter ( 0 );
    try {
        for ( uint64_t i = 0; i < N; ++i ) {
            uint64_t j = pg . index ( pg . parameter(i) );
            if ( i != j ) {
              std::cout << "Wrong index\n";
              std::cout << "The index was " << i << " but returned " << j << "\n";
            }
        }

    } catch ( ... ) {}
    try {

      for ( uint64_t i=0; i<N; ++ i ) {
        std::cout << "\n\nParameter index : " << i << "\n";
        auto adj = pg . adjacencies ( i );
        std::cout << "\n\n";
        std::cout << pg. parameter (i) . inequalities ();
        //
        std::cout << "\nadjacent parameter index : ";
        for ( auto j : adj ) {
          std::cout << "\n\n" << j << "\n\n ";

            std::cout << pg. parameter (j) . inequalities ();

        }
        std::cout << "\n";
        std::cout << "\n---------------------------\n";
        std::cout << "adjacent parameter index : ";
        for ( auto j : adj ) std::cout << j << " ";
        std::cout << "\n---------------------------\n";
      }

    } catch ( ... ) {}

    //
    //
    try {
      Network net;

    } catch ( ... ) {}
    //
    //
    try {
      Network net ( "networks/network4.txt" );
      ParameterGraph pg1 ( net );
    } catch ( ... ) {}
    boost::archive::text_oarchive oa(std::cout);
    oa << pg;
  } catch ( std::exception & e ) {
    std::cout << e . what () << "\n";
    return 1;
  }
  return 0;
}
