/// TestMorseDecomposition.cpp
/// Shaun Harker
/// 2015-05-24

#include "common.h"
#include "DSGRN.h"

int main ( int argc, char * argv [] ) {
  try {
    // Load a network file
    Network network ( "../networks/5D_Cycle.txt" );

    // Construct a parameter graph
    std::cout << "Construct parameter graph.\n";
    ParameterGraph pg ( network );

    // Fetch a random parameter from the parameter graph
    std::cout << "Choosing parameter.\n";
    uint64_t N = pg . size ();
    uint64_t param_index = 150015529;
    Parameter param = pg. parameter ( param_index );
    std::cout << "Chose " << param << "\n";

    // Construct the domain graph
    std::cout << "Construct domain graph.\n";
    DomainGraph dg ( param );

    // Construct the wall graph
    std::cout << "Construct wall graph.\n";
    WallGraph defaultwg; // coverage
    WallGraph wg ( param );

    // Compute the Morse Decomposition
    std::cout << "Construct Morse Decomposition.\n";
    MorseDecomposition wmd ( wg . digraph () );

    // Construct Morse Graph
    std::cout << "Construct Morse Graph\n";
    MorseGraph wmg ( wg, wmd );

    // Compute the Morse Decomposition
    std::cout << "Construct Morse Decomposition.\n";
    MorseDecomposition md ( dg . digraph () );

    // Construct Morse Graph
    std::cout << "Construct Morse Graph\n";
    MorseGraph mg ( dg, md );

    // << a graphviz file for the poset
    std::cout << mg;

    // Print the SHA-256 code
    std::cout << "The SHA-256 code for the Morse Graph is \n  " << mg . SHA256() << "\n";
    std::cout << "The SHA-256 code for the (wall) Morse Graph is \n  " << wmg . SHA256() << "\n";

    // Print the Morse Graph JSON
    std::cout << "JSON:\n";
    std::cout << mg . stringify () << "\n";
    std::cout << wmg . stringify () << "\n";

    MorseGraph mg2;
    mg2 . parse ( mg . stringify () );
    std::cout << "Reinputting the output:\n";
    std::cout << mg2 . stringify () << "\n";

    boost::archive::text_oarchive oa(std::cout);
    oa << mg;
  } catch ( std::exception & e ) {
    std::cout << e . what () << "\n";
    return 1;
  }
  return 0;
}
