#include <iostream>
#include "MatchingGraph.h"

int main ( int argc, char * argv [] ) {
  try {
    // Build pattern
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
    // Construct network
    Network network;
    network . assign ( "X : X + Y \n" 
                       "Y : ~X    \n" ); 
    // Construct parameter graph
    ParameterGraph parametergraph ( network );
    // Construct parameter
    uint64_t pi = 31; // parameter index 31 corresponds to '[["X",[2,2,"D5"],[0,1]],["Y",[1,1,"2"],[0]]]'
    Parameter p = parametergraph . parameter ( pi );
    // Construct domain graph
    DomainGraph dg ( p );
    // Construct search graph
    SearchGraph sg ( dg, 0 );
    // Construct matching graph
    MatchingGraph mg ( sg, pg );
    // Print search graph
    std::string output = mg . graphviz ();
    std::cout << output;
    // if ( output != expected_output ) {
    //   throw std::logic_error("Did not get expected output");
    // }
  } catch ( std::exception & e ) {
    std::cout << "Failed: " << e . what () << "\n";
    return 1;
  }
  return 0;
}
