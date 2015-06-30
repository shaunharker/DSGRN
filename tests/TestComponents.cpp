/// TestComponents.cpp
/// Shaun Harker
/// 2015-06-30

#include "common.h"
#include "DSGRN.h"

int main ( int argc, char * argv [] ) {
  try { 
    std::vector<uint64_t> vertices = {0,1,2,3,4,5};
    std::vector<bool> components =   {1,0,0,0,1,0};
    std::vector<bool> recurrent  =   {0,1};
    Components c0;
    Components c ( vertices, components, recurrent );
    if ( c [ 0 ] . size () != 4 ) throw std::runtime_error ( "Component bug 1" );
    if ( c [ 1 ] . size () != 2 ) throw std::runtime_error ( "Component bug 2" );
    if ( c . size () != 2 ) throw std::runtime_error ( "Component bug 3" );
    if ( c . recurrentComponents () [ 0 ] . size () != 2 ) throw std::runtime_error ( "Component bug 4" );
    if ( c . whichComponent ( 4 ) != 1 ) throw std::runtime_error ( "Component bug 5" );
    if ( c . isRecurrent ( 0 ) ) throw std::runtime_error ( "Component bug 6" );
    std::cout << c << "\n";
    std::ostringstream oss;
    {
      boost::archive::text_oarchive oa(oss);
      oa << c;
    }
    std::istringstream iss ( oss . str () );
    { 
      boost::archive::text_iarchive ia(iss);
      ia >> c;
    }
  } catch ( std::exception & e ) {
    std::cout << e . what () << "\n";
    return 1;
  }
  return 0;
}
