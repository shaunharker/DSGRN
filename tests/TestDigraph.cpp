/// TestDigraph.cpp
/// Shaun Harker
/// 2015-06-30

#include "common.h"
#include "DSGRN.h"

int main ( int argc, char * argv [] ) {
  try { 
    Digraph dg;
    uint64_t u = dg . add_vertex ();
    uint64_t v = dg . add_vertex ();
    dg . add_edge ( u, v );
    dg . finalize ();
    if ( dg . adjacencies ( u ) [ 0 ] != v ) throw std::runtime_error ( "Digraph::adjacencies bug" );
    Digraph const& cdg = dg;
    if ( cdg . adjacencies ( u ) [ 0 ] != v ) throw std::runtime_error ( "Digraph::adjacencies bug" );
    if ( dg . size () != 2 ) throw std::runtime_error ( "Digraph::size bug" );
    dg . resize ( 3 );
    if ( dg . size () != 3 ) throw std::runtime_error ( "Digraph::resize bug" );
    Digraph dg2;
    dg2 . parse ( dg . stringify () );
    if ( dg2 . size () != 3 ) throw std::runtime_error ( "Digraph bug (stringify,parse,or size)" );
    std::cout << dg;
    boost::archive::text_oarchive oa(std::cout);
    oa << dg;
  } catch ( std::exception & e ) {
    std::cout << e . what () << "\n";
    return 1;
  }
  return 0;
}
