/// TestNetwork.cpp
/// Shaun Harker
/// 2015-05-22

#include "common.h"
#include "DSGRN.h"

/* Test file:
X : (X + Y3) ~Z ~W1 : X Y1 Z
Y1 : X : Y2 
Y2 : (Y1 + W1) : Y3 W1
Y3 : Y2(~W1) : X
. this is a comment
Z : X(~W2) : X
@ this is a comment
W1 : Y2(~W2) : X Y2 Y3 W2
W2 : W1 : Z W1
. hello

. why not have a blank space?
*/
int main ( int argc, char * argv [] ) {
  try {
    // Load network
    Network network;
    std::string testfile;
    if ( argc < 2 ) testfile = "networks/network1.txt";
    else testfile = argv[1];
    network . load ( testfile );
    Network network2 ( testfile );
    if ( network . size () != 7 ) throw std::runtime_error ( "Network::size bug");
    if ( network . index ( "Y2" ) != 2 ) throw std::runtime_error ( "Network::index bug" );
    if ( network . name ( 5 ) != "W1" ) throw std::runtime_error ( "Network::name bug" );
    if ( network . inputs ( 0 ) . size () != 4 ) throw std::runtime_error ( "Network::inputs bug" ); 
    if ( network . outputs ( 0 ) . size () != 3 ) throw std::runtime_error ( "Network::outputs bug" ); 
    if ( network . logic ( 0 ) . size () != 3 || network . logic ( 0 ) [2] . size () != 2) throw std::runtime_error ( "Network::logic bug" );
    if ( network . interaction ( 4, 0 ) ) throw std::runtime_error ( "Network::interaction bug" ); 
    if ( network . order ( 5, 2 ) != 1 ) throw std::runtime_error ( "Network::order bug" ); 
    if ( network . domains () . size () != 7 ) throw std::runtime_error ( "Network::domains bug");
    std::cout << network . specification () << "\n";
    std::cout << network . graphviz () << "\n";
    std::cout << network << "\n";
    boost::archive::text_oarchive oa(std::cout);
    oa << network;

    Network dbnet ( "networks/network5.db" );
    try { 
      Network net ( "networks/network6" ); // doesn't exist
      return 1;
    } catch ( ... ) {}
    try {
      Network net ( "networks/network6.txt" ); // invalid network
      return 1;
    } catch ( ... ) {}
    try {
      Network net ( "networks/network7.txt" ); // invalid network
      return 1;
    } catch ( ... ) {}    
    // Test 
  } catch ( std::exception & e ) {
    std::cout << e . what () << "\n";
    return 1;
  }
  return 0;
}
