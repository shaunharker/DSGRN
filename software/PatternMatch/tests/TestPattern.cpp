#include <iostream>
#include "Pattern.h"

int main ( int argc, char * argv [] ) {
  try {
    Pattern default_pattern;
    Digraph digraph;
    digraph . resize ( 4 );
    digraph . add_edge ( 0, 2 );
    digraph . add_edge ( 1, 3 );
    std::unordered_map<uint64_t,uint64_t> events;
    events[0] = 0; events[2] = 1;
    events[1] = 1; events[3] = 0;
    uint64_t label = 3;
    Poset poset ( digraph );
    Pattern pattern ( poset, events, label, 2);
    auto fail = []( std::string const& s) { throw std::logic_error ( s ); };
    if ( pattern . label() != label ) fail ( "pattern::label failed" );
    if ( pattern . dimension() != 2 ) fail ( "pattern::dimension failed");
    if ( pattern . poset() . size() != 4 ) fail ( "pattern::poset failed");
    if ( pattern . event(2) != 1 ) fail ( "pattern::event failed");
  } catch ( std::exception & e ) {
    std::cout << e . what () << "\n";
    return 1;
  }
  return 0;
}
