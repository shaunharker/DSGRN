/// TestPatternGraph.cpp
/// Shaun Harker
/// MIT LICENSE 2016

#include "common.h"
#include "DSGRN.h"

std::string expected_output = 
  "digraph {\n"
  "0[label=\"0:DD(leaf)\"];\n"
  "1[label=\"1:ID\"];\n"
  "2[label=\"2:DI\"];\n"
  "3[label=\"3:DD\"];\n"
  "4[label=\"4:II\"];\n"
  "5[label=\"5:DI\"];\n"
  "6[label=\"6:ID\"];\n"
  "7[label=\"7:DD(root)\"];\n"
  "8[label=\"8:DD\"];\n"
  "0 -> 0 [label=\"--\"];\n"
  "1 -> 0 [label=\"M-\"];\n"
  "1 -> 1 [label=\"--\"];\n"
  "2 -> 0 [label=\"-M\"];\n"
  "2 -> 2 [label=\"--\"];\n"
  "3 -> 2 [label=\"-m\"];\n"
  "3 -> 3 [label=\"--\"];\n"
  "4 -> 2 [label=\"M-\"];\n"
  "4 -> 1 [label=\"-M\"];\n"
  "4 -> 4 [label=\"--\"];\n"
  "5 -> 8 [label=\"-M\"];\n"
  "5 -> 4 [label=\"m-\"];\n"
  "5 -> 5 [label=\"--\"];\n"
  "6 -> 3 [label=\"M-\"];\n"
  "6 -> 4 [label=\"-m\"];\n"
  "6 -> 6 [label=\"--\"];\n"
  "7 -> 6 [label=\"m-\"];\n"
  "7 -> 5 [label=\"-m\"];\n"
  "7 -> 7 [label=\"--\"];\n"
  "8 -> 1 [label=\"m-\"];\n"
  "8 -> 8 [label=\"--\"];\n"
  "}\n";

int main ( int argc, char * argv [] ) {
  try {
    PatternGraph default_patterngraph_test;
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
    std::string output = pg . graphviz ();
    std::cout << output;
    if ( output != expected_output ) {
      throw std::logic_error("Did not get expected output");
    }
  } catch ( std::exception & e ) {
    std::cout << e . what () << "\n";
    return 1;
  }
  return 0;
}
