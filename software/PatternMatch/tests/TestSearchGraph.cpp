#include <iostream>
#include "SearchGraph.h"

std::string expected_output = 
  "digraph {\n"
  "0[label=\"0:I?\"];\n"
  "1[label=\"1:?D\"];\n"
  "2[label=\"2:D?\"];\n"
  "3[label=\"3:?I\"];\n"
  "0 -> 1 [label=\"1\"];\n"
  "1 -> 2 [label=\"0\"];\n"
  "2 -> 3 [label=\"1\"];\n"
  "3 -> 0 [label=\"0\"];\n"
  "}\n";

int main ( int argc, char * argv [] ) {
  try {
    // Construct network
    Network network;
    network . assign ( "X : X + Y \n" 
                       "Y : ~X    \n" ); 
    // Construct parameter graph
    ParameterGraph pg ( network );
    // Construct parameter
    uint64_t pi = 31; // parameter index 31 corresponds to '[["X",[2,2,"D5"],[0,1]],["Y",[1,1,"2"],[0]]]'
    Parameter p = pg . parameter ( pi );
    // Construct domain graph
    DomainGraph dg ( p );
    // Construct search graph
    SearchGraph sg ( dg, 0 );
    // Print search graph
    std::string output = sg . graphviz ();
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

/*
dsgrn network testnetwork.txt domaingraph graphviz '[["X",[2,2,"D5"],[0,1]],["Y",[1,1,"2"],[0]]]'
digraph {
0;
1;
2;
3;
4;
5;
0 -> 1;
0 -> 3;
1 -> 4;
2 -> 1;
3 -> 4;
4 -> 5;
5 -> 2;
}
*/