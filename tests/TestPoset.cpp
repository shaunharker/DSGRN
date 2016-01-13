/// TestDigraph.cpp
/// Shaun Harker
/// 2015-06-30

#include "common.h"
#include "DSGRN.h"

int main ( int argc, char * argv [] ) {
  try {

    Poset ps;

    uint64_t n0 = ps . add_vertex ();
    uint64_t n1 = ps . add_vertex ();
    uint64_t n2 = ps . add_vertex ();
    uint64_t n3 = ps . add_vertex ();
    uint64_t n4 = ps . add_vertex ();
    uint64_t n5 = ps . add_vertex ();
    uint64_t n6 = ps . add_vertex ();

    ps . add_edge ( n0, n1 );
    ps . add_edge ( n0, n2 );
    ps . add_edge ( n1, n3 );
    ps . add_edge ( n2, n3 );
    ps . add_edge ( n3, n4 );

    ps . add_edge ( n0, n3 );
    ps . add_edge ( n4, n5 );
    ps . add_edge ( n4, n6 );

    std::cout << ps;

    ps . reduction ( );

    for ( uint64_t i=0; i<ps.size(); ++i ) {
      std::cout << "Node " << i << "\n";
      std::cout << "#ancestors   : " << ps.numberOfAncestors(i) << "\n";
      std::cout << "#descendants : " << ps.numberOfDescendants(i) << "\n";
      std::cout << "#parents     : " << ps.numberOfParents(i) << "\n";
      std::cout << "#children    : " << ps.numberOfChildren(i) << "\n";
    }

    std::cout << ps;
    boost::archive::text_oarchive oa(std::cout);
    oa << ps;


  } catch ( std::exception & e ) {
    std::cout << e . what () << "\n";
    return 1;
  }
  return 0;
}
