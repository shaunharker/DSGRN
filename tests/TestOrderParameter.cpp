/// OrderParameter.cpp
/// Shaun Harker
/// 2015-05-24

#include <iostream>
#include <vector>
#include "Parameter/OrderParameter.h"

int main ( void ) {
  OrderParameter p;
  p . assign ( 10, 123456 );
  std::vector<uint64_t> perm = p . permutation ();
  OrderParameter q;
  q . assign ( perm );
  if ( q . index () != 123456 ) {
    std::cout << "OrderParameter test FAILED.\n";
    return 1;
  }
  std::cout << "OrderParameter test passed.\n";
  return 0;
}
