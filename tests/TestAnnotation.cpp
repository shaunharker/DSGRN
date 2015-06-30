/// TestAnnotation.cpp
/// Shaun Harker
/// 2015-06-30

#include "common.h"
#include "DSGRN.h"

int main ( int argc, char * argv [] ) {
  try {
    Annotation a;
    a . append ( "X" );
    a . append ( "Y" );
    if ( a . size () != 2 ) return 1;
    if ( a [1] != "Y" ) return 1;
    for ( std::string const& s : a ) {
      std::cout << a << "\n";
    }
    Annotation b;
    b . parse ( a . stringify () );
    std::cout << b << "\n";
    boost::archive::text_oarchive oa(std::cout);
    oa << b;
  } catch ( std::exception & e ) {
    std::cout << e . what () << "\n";
    return 1;
  }
  return 0;
}
