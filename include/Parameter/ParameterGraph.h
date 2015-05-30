/// ParameterGraph.h
/// Shaun Harker
/// 2015-05-24

#ifndef DSGRN_PARAMETERGRAPH_H
#define DSGRN_PARAMETERGRAPH_H

#include <iostream>
#include <memory>
#include <cstdlib>
#include <vector>
#include <string>
#include <sstream>
#include <exception>
#include <stdexcept>

#include "DSGRN.h"

class ParameterGraph_ {
public:

  /// assign
  ///   Assign a network to the parameter graph
  ///   Search in path for logic .dat files
  void
  assign ( Network const& network, 
           std::string const& path = std::string("./data/logic/") );

  /// size
  ///   Return the number of parameters
  uint64_t
  size ( void ) const;

  /// parameter
  ///   Return the parameter associated with an index
  Parameter
  parameter ( uint64_t index ) const;

  /// index
  ///   Return the index associated with a parameter
  uint64_t
  index ( Parameter const& p ) const;

  /// adjacencies
  ///   Return the adjacent parameters to a given parameter
  std::vector<uint64_t>
  adjacencies ( uint64_t index ) const;

  /// network
  ///   Return network
  Network const
  network ( void ) const;

  /// fixedordersize
  ///   Return the number of parameters
  ///   for a fixed ordering
  uint64_t
  fixedordersize ( void ) const;

  /// reorderings
  ///   Return of reorderings
  ///   Note: size() = fixedordersize()*reorderings()
  uint64_t
  reorderings ( void ) const;

  /// operator <<
  ///   Stream out information about parameter graph.
  friend std::ostream& operator << ( std::ostream& stream, ParameterGraph_ const& pg );

private:
  Network network_;
  uint64_t size_;
  uint64_t reorderings_;
  uint64_t fixedordersize_;
  std::vector<uint64_t> logic_place_values_;
  std::vector<uint64_t> order_place_values_;
  std::vector<std::vector<std::string>> factors_;
  uint64_t _factorial ( uint64_t m ) const;
};


inline void ParameterGraph_::
assign ( Network const& network, std::string const& path ) {
  network_ = network;
  reorderings_ = 1;
  fixedordersize_ = 1;
  // Load the network files one by one.
  uint64_t D = network_ . size ();
  for ( uint64_t d = 0; d < D; ++ d ) {
    uint64_t n = network_ . inputs ( d ) . size ();
    uint64_t m = network_ . outputs ( d ) . size ();
    order_place_values_ . push_back ( _factorial ( m ) );
    reorderings_ *= order_place_values_ . back ();
    std::vector<std::vector<uint64_t>> const& logic_struct = network_ . logic ( d );
    std::stringstream ss;
    ss << path << n << "_" << m;
    for ( auto const& p : logic_struct ) ss << "_" << p.size();
    ss << ".dat";
    //std::cout << "Acquiring logic data in " << ss.str() << "\n";
    std::vector<std::string> hex_codes;
    std::ifstream infile ( ss.str() );
    if ( not infile . good () ) { 
      throw std::runtime_error ( "Error: Could not find logic resource " + ss.str() + ".\n");
    }
    std::string line;
    while ( std::getline ( infile, line ) ) hex_codes . push_back ( line );
    infile . close ();
    factors_ . push_back ( hex_codes );
    logic_place_values_ . push_back ( hex_codes . size () );
    fixedordersize_ *= hex_codes . size ();
  }
  size_ = fixedordersize_ * reorderings_;
}

inline uint64_t ParameterGraph_::
size ( void ) const {
  return size_;
}

inline Parameter ParameterGraph_::
parameter ( uint64_t index ) const {
  //std::cout << "ParameterGraph_::parameter( " << index << " )\n";
  uint64_t logic_index = index % fixedordersize_;
  uint64_t order_index = index / fixedordersize_;
  //std::cout << "Logic index = " << logic_index << "\n";
  //std::cout << "Order index = " << order_index << "\n";

  uint64_t D = network_ . size ();
  std::vector<uint64_t> logic_indices;
  for ( uint64_t d = 0; d < D; ++ d ) {
    uint64_t i = logic_index % logic_place_values_ [ d ];
    logic_index /= logic_place_values_ [ d ];
    logic_indices . push_back ( i );
  }
  std::vector<uint64_t> order_indices;
  for ( uint64_t d = 0; d < D; ++ d ) {
    uint64_t i = order_index % order_place_values_ [ d ];
    order_index /= order_place_values_ [ d ];
    order_indices . push_back ( i );
  }
  std::vector<LogicParameter> logic;
  std::vector<OrderParameter> order;
  for ( uint64_t d = 0; d < D; ++ d ) {
    uint64_t n = network_ . inputs ( d ) . size ();
    uint64_t m = network_ . outputs ( d ) . size ();
    std::string hex_code = factors_ [ d ] [ logic_indices[d] ];
    LogicParameter logic_param;
    logic_param . assign ( n, m, hex_code );
    OrderParameter order_param;
    order_param . assign ( m, order_indices[d] );
    logic . push_back ( logic_param );
    order . push_back ( order_param );
  }
  Parameter result;
  result . assign ( logic, order, network_ );
  return result;
}

inline uint64_t ParameterGraph_::
index ( Parameter const& p ) const {
  // TODO
  throw std::runtime_error ( "Feature not implemented" );
  return 0;
}

inline std::vector<uint64_t> ParameterGraph_::
adjacencies ( uint64_t index ) const {
  //TODO
  throw std::runtime_error ( "Feature not implemented" );
  return std::vector<uint64_t> ();
}

inline Network const ParameterGraph_::
network ( void ) const {
  return network_;
}

inline std::ostream& operator << ( std::ostream& stream, ParameterGraph_ const& pg ) {
  stream << "(ParameterGraph_: "<< pg.size() << " parameters, " 
         << pg.network().size() << " nodes)";
  return stream;
}

inline uint64_t ParameterGraph_::
fixedordersize ( void ) const {
  return fixedordersize_;
}

inline uint64_t ParameterGraph_::
reorderings ( void ) const {
  return reorderings_;
}

inline uint64_t ParameterGraph_::
_factorial ( uint64_t m ) const {
  static const std::vector<uint64_t> table = 
    { 1, 1, 2, 6, 24, 120, 600, 3600, 25200, 201600};
  if ( m < 10 ) return table [ m ];
  return m * _factorial ( m - 1 );
}


#endif
