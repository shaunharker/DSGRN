/// TestWall.cpp
/// Shaun Harker
/// 2015-06-30

#include "common.h"
#include "DSGRN.h"

int main ( int argc, char * argv [] ) {
  try { 
    Domain d;
    std::vector<uint64_t> limits = {5,5,5};
    Wall w1;
    Wall w2 ( limits, 0, 1 );
    w1 . assign ( limits, 0, 1 );
    if ( w1 . index () != 9 ) return 1;
    std::cout << w1 << "\n";
    boost::archive::text_oarchive oa(std::cout);
    oa << w1;
  } catch ( std::exception & e ) {
    std::cout << e . what () << "\n";
    return 1;
  }
  return 0;
}
