/// Network.h
/// Shaun Harker
/// 2015-05-22

#ifndef DSGRN_NETWORK_H
#define DSGRN_NETWORK_H

#include <algorithm>
#include <exception>
#include <stdexcept>
#include <cctype>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <sstream>
#include <fstream>
#include <utility>

#include <boost/functional/hash.hpp>

/// Network
///   This class holds network data.
///     * Loads specification files
///     * Outputs Graphviz visualizations
///     * Provides an interface to networks
class Network {
public:
  /// load
  ///   load from network specification file
  void 
  load ( std::string const& filename );

  /// size
  ///   Return the number of nodes in the network
  uint64_t
  size ( void ) const;

  /// index
  ///   Return index of node given name string
  uint64_t 
  index ( std::string const& name ) const;

  /// name
  ///   Return name of node (given by index)
  std::string const&
  name ( uint64_t index ) const;

  /// inputs
  ///   Return a list of inputs to a node
  std::vector<uint64_t> const&
  inputs ( uint64_t index ) const;

  /// outputs
  ///   Return a list of outputs to a node
  std::vector<uint64_t> const&
  outputs ( uint64_t index ) const;

  /// logic
  ///   Return the logic of a node (given by index)
  std::vector<std::vector<uint64_t>> const&
  logic ( uint64_t index ) const;

  /// uint64_teraction
  ///   Return the uint64_teraction type of an edge:
  ///   False for repression, true for activation
  bool
  interaction ( uint64_t source, uint64_t target ) const;

  /// order
  ///   Return the out-edge order number of an edge, i.e. so
  ///   outputs(source)[order(source,target)] == target
  uint64_t
  order ( uint64_t source, uint64_t target ) const;

  /// domains
  ///   Return a list consisting of the number of 
  ///   domains across (i.e. number of out-edges plus one)
  ///   for each dimension (i.e. network node)
  std::vector<uint64_t>
  domains ( void ) const;

  /// graphviz
  ///   Return a graphviz string (dot language)
  std::string
  graphviz ( std::vector<std::string> const& theme = 
  { "aliceblue", // background color
    "beige",     // node color
    "black", "darkgoldenrod", "blue", "orange", "red", "yellow" // edge group colors
  } ) const;

private:
  std::vector<std::vector<uint64_t>> inputs_;
  std::vector<std::vector<uint64_t>> outputs_;
  std::unordered_map<std::string, uint64_t> index_by_name_;
  std::vector<std::string> name_by_index_;
  std::unordered_map<std::pair<uint64_t,uint64_t>, bool, boost::hash<std::pair<uint64_t,uint64_t>>> edge_type_;
  std::unordered_map<std::pair<uint64_t,uint64_t>, uint64_t, boost::hash<std::pair<uint64_t,uint64_t>>> order_;
  std::vector<std::vector<std::vector<uint64_t>>> logic_by_index_;
  std::vector<std::string> _lines ( std::string const& filename );
  void _parse ( std::vector<std::string> const& lines );
};


inline void Network::
load ( std::string const& filename ) {
  _parse ( _lines ( filename ) );
}

inline uint64_t Network::
size ( void ) const {
  return name_by_index_ . size ();
}

inline uint64_t Network::
index ( std::string const& name ) const {
  return index_by_name_ . find ( name ) -> second;
}

inline std::string const& Network::
name ( uint64_t index ) const {
  return name_by_index_[index];
}

inline std::vector<uint64_t> const& Network:: 
inputs ( uint64_t index ) const {
  return inputs_[index];
}

inline std::vector<uint64_t> const& Network:: 
outputs ( uint64_t index ) const {
  return outputs_[index];
}

inline std::vector<std::vector<uint64_t>> const& Network::
logic ( uint64_t index ) const {
  return logic_by_index_ [ index ];
}

inline bool Network::
interaction ( uint64_t source, uint64_t target ) const {
  return edge_type_ . find ( std::make_pair ( source, target ) ) -> second;
}

inline uint64_t Network::
order ( uint64_t source, uint64_t target ) const {
  return order_ . find ( std::make_pair ( source, target ) ) -> second;
}

inline  std::vector<uint64_t> Network::
domains ( void ) const {
  std::vector<uint64_t> result;
  for ( auto const& output : outputs_ ) {
    result . push_back ( output . size () + 1);
  }
  return result;
}
  
inline std::string Network::
graphviz ( std::vector<std::string> const& theme ) const {
  std::stringstream result;
  // std::cout << "graphviz. Looping through nodes.\n";
  result << "digraph {\n";
  result << "bgcolor = " << theme[0] << ";";
  for ( uint64_t i = 0; i < size (); ++ i ) {
    result << name(i) << "[style=filled fillcolor=" << theme[1] << "];\n";
  }
  std::string normalhead ("normal");
  std::string blunthead ("tee");
  // std::cout << "graphviz. Looping through edges.\n";
  for ( uint64_t target = 0; target < size (); ++ target ) {
    std::vector<std::vector<uint64_t>> logic_struct = logic ( target );
    std::reverse ( logic_struct . begin (), logic_struct . end () ); // prefer black
    uint64_t partnum = 0;
    for ( auto const& part : logic_struct ) {
      for ( uint64_t source : part ) {
        // std::cout << "Checking type of edge from " << source << " to " << target << "\n";
        std::string head = interaction(source,target) ? normalhead : blunthead;
        result << name(source) << " -> " << name(target) << "[color=" << theme[partnum+2] << " arrowhead=\"" << head << "\"];\n";
      }
      ++ partnum;
      if ( partnum + 2 == theme . size () ) partnum = 0;
    }
  }  
  result << "}\n";
  return result . str ();
}


namespace DSGRN_parse_tools {
  // http://stackoverflow.com/questions/236129/split-a-string-in-c
  inline std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
  }
  inline std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, elems);
    return elems;
  }
  inline std::string& removeSpace ( std::string &s ) {
    s.erase(std::remove_if(s.begin(), s.end(), (uint64_t(*)(uint64_t))isspace), s.end());
    return s;
  }
}

/// lines
///   Open the network file and read it line by line
inline std::vector<std::string> Network::
_lines ( std::string const& filename ) {
  std::vector<std::string> result;
  std::ifstream infile ( filename );
  if ( not infile . good () ) { 
    throw std::runtime_error ( "Problem loading network file " + filename + "\n");
  }
  std::string line;
  while ( std::getline ( infile, line ) ) {
    result . push_back ( line );
  }
  infile . close ();
  return result;
}

/// parse
///   Iterate through lines and produce data structures
inline void Network::
_parse ( std::vector<std::string> const& lines ) {
  using namespace DSGRN_parse_tools;
  std::vector<std::string> logic_strings;
  //std::vector<std::string> constrauint64_t_strings;
  // Learn the node names
  for ( auto const& line : lines ) {
    auto splitline = split ( line, ':' );
    if ( splitline . empty () ) continue;
    removeSpace(splitline[0]);
    // If begins with . or @, skip
    if ( (splitline[0][0] == '.') || (splitline[0][0] == '@' ) ) continue; 
    name_by_index_ . push_back ( splitline[0] );
    // This is a network node description line, so it should have a logic:
    if ( splitline . size () < 2 ) {
      throw std::runtime_error ( "Problem with network file specification: missing logic.\n");
    }
    logic_strings . push_back ( splitline[1] );
  }
  // Index the node names
  uint64_t loop_index = 0;
  for ( auto const& name :  name_by_index_ ) 
    index_by_name_ [ name ] = loop_index ++;
  // Learn the logics
  // Trick: ignore everything but node names and +'s. 
  // Example: a + ~ b c d + e  corresponds to (a+b)(b)(c)(d+e)
  uint64_t target = 0;
  for ( auto const& logic_string : logic_strings ) {
    //std::cout << "Processing " << logic_string << "\n";
    std::vector<std::vector<uint64_t>> logic_struct;
    std::vector<uint64_t> factor;
    std::string token;
    bool parity = true;
    bool appending = true;

    auto flush_factor = [&] () {
      if ( factor . empty () ) return;
      // Put factor into canonical ordering
      std::sort ( factor.begin(), factor.end() );
      logic_struct . push_back ( factor );
      //std::cout << "    Flushing factor ";
      //for ( uint64_t i : factor ) std::cout << name ( i ) << " ";
      //std::cout << "\n";
      factor . clear ();      
    };
    auto flush_token = [&] () {
      if ( token . empty () ) return;
      if ( not appending ) flush_factor ();
      //std::cout << "  Flushing token " << token << "\n";
      uint64_t source = index_by_name_ [ token ];
      factor . push_back ( source );
      edge_type_[std::make_pair( source, target )] = parity;
      //std::cout << "Creating edge from " << source << " to " << target << "\n";
      token . clear ();
      appending = false;
      parity = true;
    };
    for ( char c : logic_string ) {
      //std::cout << "Reading character " << c << "\n";
      if ( (c == ' ') || (c == '(') || (c == ')') || (c == '+') || (c == '~') ) {
        flush_token ();
      } else {
        token . push_back ( c );
      }
      if ( c == '+' ) { 
        appending = true;
        //std::cout << "  Detected +\n";
      }
      if ( c == '~' ) parity = false;
    }
    flush_token ();
    flush_factor ();
    //std::cout << "The logic_struct formed.\n";
    // Ensure logic_struct is acceptable (no repeats!)
    std::unordered_set<uint64_t> inputs;
    for ( auto const& factor : logic_struct ) {
      //std::cout << "# ";
      for ( auto i : factor ) {
        //std::cout << i << " ";
        if ( inputs . count ( i ) ) {
          throw std::runtime_error ( "Problem with network spec: Repeated inputs in logic.\n" );
        }
        inputs . insert ( i );
      }
    }
    //std::cout << "\n";
    //std::cout << "The logic_struct is acceptable.\n";
    // Compare partitions by (size, max), where size is length and max is maximum index
    auto compare_partition = [](std::vector<uint64_t> const& lhs, std::vector<uint64_t> const& rhs) {
      if ( lhs . size () < rhs . size () ) return true;
      if ( lhs . size () > rhs . size () ) return false;
      uint64_t max_lhs = * std::max_element ( lhs.begin(), lhs.end() );
      uint64_t max_rhs = * std::max_element ( rhs.begin(), rhs.end() );
      if ( max_lhs < max_rhs ) return true;
      if ( max_lhs > max_rhs ) return false;
      throw std::logic_error ( "Thrown from Network.h\n");
      return false;
    };
    // Put the logic struct into a canonical ordering.
    std::sort ( logic_struct.begin(), logic_struct.end(), compare_partition );
    logic_by_index_ . push_back ( logic_struct );
    //std::cout << "The logic_struct has been incorporated into the network.\n";
    ++ target;
  }
  // Compute inputs and outputs.
  inputs_ . resize ( size () );
  outputs_ . resize ( size () );
  for ( target = 0; target < size (); ++ target ) {
    for ( auto const& factor : logic ( target ) ) {
      for ( uint64_t source : factor ) {
        inputs_[target] . push_back ( source );
        outputs_[source] . push_back ( target );
        order_[std::make_pair(source,target)] = outputs_[source].size()-1;
      }
    }
  }
  //std::cout << "_parse complete.\n";
}

#endif
