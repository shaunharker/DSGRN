/// TestOrderParameter.cpp
/// Shaun Harker
/// 2015-05-24

#include <iostream>
#include <vector>

#include "DSGRN.h"

int main ( void ) {
  try { 
    uint64_t m = 10;
    for ( uint64_t i = 0; i < 1000; ++ i ) {
      uint64_t k = rand () % 1000000;
      OrderParameter p ( m, k );
      std::vector<uint64_t> perm = p . permutation ();
      OrderParameter q ( perm );
      if ( q . size () != m ) throw std::runtime_error ( "Permutation size bug" );
      if ( q . index () != k ) { 
        std::cout << q . index () << " != " << k << "\n";
        std::cout << p << "\n";
        std::cout << q << "\n";
        throw std::runtime_error ( "Permutation constructor bug" );
      }
      for ( uint64_t j = 0; j < m; ++ j ) {
        if ( p . inverse ( p ( j ) ) != j )  throw std::runtime_error ( "Permutation inversion bug" );
      }
    }
    OrderParameter a;
    OrderParameter b ( 5, 5 );
    a . parse ( b . stringify () );
    std::cout << a << "\n";
    boost::archive::text_oarchive oa(std::cout);
    oa << a;
  } catch ( std::exception & e ) {
    std::cout << e . what () << "\n";
    return 1;
  }
  return 0;
}
