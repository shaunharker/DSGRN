/// TestStrongComponents.cpp
/// Shaun Harker
/// 2015-06-30

#include "common.h"
#include "DSGRN.h"

int main ( int argc, char * argv [] ) {
  try { 
    Digraph dg;
    for ( uint64_t v = 0; v < 128; ++ v ) {
      dg . add_vertex ();
    }
    for ( uint64_t v = 0; v < 128; ++ v ) {
      for ( uint64_t u = 0; u < 128; ++ u ) {
        dg . add_edge ( v, u );
      }
    }
    dg . finalize ();
    auto sccs = StrongComponents ( dg );
  } catch ( std::exception & e ) {
    std::cout << e . what () << "\n";
    return 1;
  }
  return 0;
}
