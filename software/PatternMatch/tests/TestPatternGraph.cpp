#include <iostream>
#include "PatternGraph.h"

int main ( int argc, char * argv [] ) {
  try {
    // Build pattern
    Pattern default_pattern;
    Digraph digraph;
    digraph . resize ( 4 );
    digraph . add_edge ( 0, 2 );
    digraph . add_edge ( 1, 3 );
    std::unordered_map<uint64_t,uint64_t> events;
    events[0] = 0; events[2] = 0;
    events[1] = 1; events[3] = 1;
    uint64_t label = 3;
    Poset poset ( digraph );
    Pattern pattern ( poset, events, label, 2);
    // Build pattern graph
    PatternGraph pg ( pattern );
    std::cout << pg . graphviz () << "\n";
  } catch ( std::exception & e ) {
    std::cout << e . what () << "\n";
    return 1;
  }
  return 0;
}
