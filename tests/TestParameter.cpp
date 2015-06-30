/// TestParameter.cpp
/// Shaun Harker
/// 2015-06-30

#include "common.h"
#include "DSGRN.h"

int main ( int argc, char * argv [] ) {
  try {
    Network net ( "networks/network2.txt" );
    ParameterGraph pg ( net );
    Parameter p = pg . parameter ( 43 );
    Domain dom ( net . domains () );
    dom . setIndex ( 4 );
    if ( not p . attracting ( dom ) ) throw std::runtime_error ( "Parameter::attracting bug" );
    Parameter q;
    q . parse ( p . stringify () );
    std::cout << q << "\n";
    std::cout << q . network () << "\n";
    boost::archive::text_oarchive oa(std::cout);
    oa << p;
  } catch ( std::exception & e ) {
    std::cout << e . what () << "\n";
    return 1;
  }
  return 0;
}
