#include <iostream>
#include "PatternMatch.h"

int main ( int argc, char * argv [] ) {
  if ( argc == 5 ) {
    Network network; network . load ( argv[1] );
    Pattern pattern; pattern . load ( argv[2] );
    uint64_t parameter_index = std::stoll(argv[3]);
    uint64_t morse_set_index = std::stoll(argv[4]);
    ParameterGraph parametergraph ( network );
    Parameter parameter = parametergraph . parameter ( parameter_index );
    DomainGraph domaingraph ( parameter );
    SearchGraph searchgraph ( domaingraph, morse_set_index );
    PatternGraph patterngraph ( pattern );
    MatchingGraph matchinggraph ( searchgraph, patterngraph );
    std::cout << ExplainCycleMatch ( matchinggraph ) << "\n";
    std::ofstream dg ( "domaingraph.gv" );
    dg << domaingraph . graphviz () << "\n";
    std::ofstream sg ( "searchgraph.gv" );
    sg << searchgraph . graphviz () << "\n";
    std::ofstream pg ( "patterngraph.gv");
    pg << patterngraph . graphviz () << "\n";
    std::ofstream mg ( "matchingraph.gv");
    mg << matchinggraph . graphviz () << "\n";
    return 0;
  }
  try {
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
    uint64_t morseset_index = 0;
    SearchGraph sg ( dg, morseset_index );
    // Test 1
    {
      // Build pattern
      Digraph digraph;
      digraph . resize ( 4 );
      digraph . add_edge ( 0, 2 );
      digraph . add_edge ( 1, 3 );
      std::vector<uint64_t> events ( 4 );
      events[0] = 0; events[2] = 0;
      events[1] = 1; events[3] = 1;
      uint64_t label = 3; // DD
      uint64_t dimension = 2;
      Poset poset ( digraph );
      Pattern pattern ( poset, events, label, dimension);
      // Build pattern graph
      PatternGraph pg ( pattern );
      // Construct matching graph
      MatchingGraph mg ( sg, pg );
      // Pattern Matching algorithm
      bool cycle_match = QueryCycleMatch ( mg );
      bool path_match = QueryPathMatch ( mg );
      // Confirm results
      if ( not cycle_match ) throw std::logic_error ( "Expected cycle match" );
      if ( not path_match ) throw std::logic_error ( "Expected path match" );
    }
    // Test 2
    {
      // Build pattern
      Digraph digraph;
      digraph . resize ( 4 );
      digraph . add_edge ( 0, 1 );
      digraph . add_edge ( 1, 2 );
      digraph . add_edge ( 2, 3 );
      std::vector<uint64_t> events ( 4 );
      events[0] = 0; events[1] = 0;
      events[2] = 1; events[3] = 1;
      uint64_t label = 3; // DD
      uint64_t dimension = 2;
      Poset poset ( digraph );
      Pattern pattern ( poset, events, label, dimension);
      // Build pattern graph
      PatternGraph pg ( pattern );
      // Construct matching graph
      MatchingGraph mg ( sg, pg );
      // Pattern Matching algorithm
      bool cycle_match = QueryCycleMatch ( mg );
      bool path_match = QueryPathMatch ( mg );
      // Confirm results
      if ( cycle_match ) throw std::logic_error ( "Unexpected cycle match" );
      if ( path_match ) throw std::logic_error ( "Unexpected path match" );      
    }
  } catch ( std::exception & e ) {
    std::cout << "Failed: " << e . what () << "\n";
    return 1;
  }
  return 0;
}
