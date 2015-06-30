/// TestDomain.cpp
/// Shaun Harker
/// 2015-06-30

#include "common.h"
#include "DSGRN.h"

int main ( int argc, char * argv [] ) {
  try { 
    Domain d1; // Default constructor
    std::vector<uint64_t> limits = {5,5,5};
    d1 . assign ( limits );
    Domain d2 ( limits );
    if ( (++ d1)[0] != 1 ) throw std::runtime_error ( "Domain preincrement bug" );
    if ( (d1 ++)[0] != 1 ) throw std::runtime_error ( "Domain postincrement bug" );
    if ( d1 . size () != 3 ) throw std::runtime_error ( "Domain::size bug");
    if ( d1 . index () != 2 ) throw std::runtime_error ( "Domain::index bug");
    if ( d1 . right ( 1 ) != 7 ) throw std::runtime_error ( "Domain::right bug");
    if ( d1 . left ( 0 ) != 1 ) throw std::runtime_error ( "Domain::left bug");
    if ( not d1 . isMin ( 2 ) ) throw std::runtime_error ( "Domain::isMin bug");
    d1 . setIndex ( 4 );
    if ( not d1 . isMax ( 0 ) ) throw std::runtime_error ( "Domain::isMax bug");
    std::cout << d1;
    boost::archive::text_oarchive oa(std::cout);
    oa << d1;
  } catch ( std::exception & e ) {
    std::cout << e . what () << "\n";
    return 1;
  }
  return 0;
}
