#include <iostream>
#include "MatchingGraph.h"

std::string expected_output =
  "digraph {\n"
  "0[label=\"(0,4)\"];\n"
  "1[label=\"(1,1)\"];\n"
  "2[label=\"(1,7)\"];\n"
  "3[label=\"(2,0)\"];\n"
  "4[label=\"(2,7)\"];\n"
  "5[label=\"(3,5)\"];\n"
  "0 -> 1;\n"
  "1 -> 3;\n"
  "2 -> 4;\n"
  "4 -> 5;\n"
  "5 -> 0;\n"
  "}\n";

int main ( int argc, char * argv [] ) {
  try {
    // Build pattern
    Digraph digraph;
    digraph . resize ( 4 );
    digraph . add_edge ( 0, 2 );
    digraph . add_edge ( 1, 3 );
    std::vector<uint64_t> events ( 4 );
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
    // Print matching graph
    std::string output = mg . graphviz ();
    std::cout << output;
    if ( output != expected_output ) {
      throw std::logic_error("Did not get expected output");
    }
  } catch ( std::exception & e ) {
    std::cout << "Failed: " << e . what () << "\n";
    return 1;
  }
  return 0;
}
