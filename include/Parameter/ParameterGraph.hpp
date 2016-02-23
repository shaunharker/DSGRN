/// ParameterGraph.hpp
/// Shaun Harker
/// 2015-05-24

#ifndef DSGRN_PARAMETERGRAPH_HPP
#define DSGRN_PARAMETERGRAPH_HPP

#ifndef INLINE_IF_HEADER_ONLY
#define INLINE_IF_HEADER_ONLY
#endif

#include "ParameterGraph.h"

INLINE_IF_HEADER_ONLY ParameterGraph::
ParameterGraph ( void ) {
  data_ . reset ( new ParameterGraph_ );
}

INLINE_IF_HEADER_ONLY ParameterGraph::
ParameterGraph ( Network const& network, std::string const& path ) {
  assign ( network, path );
}

INLINE_IF_HEADER_ONLY void ParameterGraph::
assign ( Network const& network, std::string const& path ) {
  data_ . reset ( new ParameterGraph_ );
  data_ -> network_ = network;
  data_ -> reorderings_ = 1;
  data_ -> fixedordersize_ = 1;
  // Load the network files one by one.
  uint64_t D = data_ -> network_ . size ();
  for ( uint64_t d = 0; d < D; ++ d ) {
    uint64_t n = data_ -> network_ . inputs ( d ) . size ();
    uint64_t m = data_ -> network_ . outputs ( d ) . size ();
    data_ -> order_place_values_ . push_back ( _factorial ( m ) );
    data_ -> reorderings_ *= data_ -> order_place_values_ . back ();
    std::vector<std::vector<uint64_t>> const& logic_struct = data_ -> network_ . logic ( d );
    std::stringstream ss;
    ss << path << "/" << n <<  "_" << m;
    for ( auto const& p : logic_struct ) ss <<  "_" << p.size();
    if ( data_ -> network_ . essential ( d ) ) ss << "_E";
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
    data_ -> factors_ . push_back ( hex_codes );
    data_ -> logic_place_values_ . push_back ( hex_codes . size () );
    data_ -> fixedordersize_ *= hex_codes . size ();
    //std::cout << d << ": " << hex_codes . size () << " factorial(" << m << ")=" << _factorial ( m ) << "\n";
  }
  data_ -> size_ = data_ -> fixedordersize_ * data_ -> reorderings_;
}

INLINE_IF_HEADER_ONLY uint64_t ParameterGraph::
size ( void ) const {
  return data_ -> size_;
}

INLINE_IF_HEADER_ONLY Parameter ParameterGraph::
parameter ( uint64_t index ) const {
  //std::cout << data_ -> "ParameterGraph::parameter( " << index << " )\n";
  if ( index >= size () ) {
    throw std::runtime_error ( "ParameterGraph::parameter Index out of bounds");
  }
  uint64_t logic_index = index % data_ -> fixedordersize_;
  uint64_t order_index = index / data_ -> fixedordersize_;
  //std::cout << "Logic index = " << logic_index << "\n";
  //std::cout << "Order index = " << order_index << "\n";

  uint64_t D = data_ -> network_ . size ();
  std::vector<uint64_t> logic_indices;
  for ( uint64_t d = 0; d < D; ++ d ) {
    uint64_t i = logic_index % data_ -> logic_place_values_ [ d ];
    logic_index /= data_ -> logic_place_values_ [ d ];
    logic_indices . push_back ( i );
  }
  std::vector<uint64_t> order_indices;
  for ( uint64_t d = 0; d < D; ++ d ) {
    uint64_t i = order_index % data_ -> order_place_values_ [ d ];
    order_index /= data_ -> order_place_values_ [ d ];
    order_indices . push_back ( i );
  }

  std::cout << "\n-----------------------\n";
  std::cout << "Method ParameterGraph::Parameter \n";
  std::cout << "logic indices : ";
  for ( auto i : logic_indices ) std::cout << i << " ";
  std::cout << "\n";
  std::cout << "logic_place_values_ : ";
  for ( auto i : data_ -> logic_place_values_ ) std::cout << i << " ";
  std::cout << "\n";
  std::cout << "order indices : ";
  for ( auto i : order_indices ) std::cout << i << " ";
  std::cout << "\n";
  std::cout << "order_place_values_ : ";
  for ( auto i : data_ -> order_place_values_ ) std::cout << i << " ";
  std::cout << "\n-----------------------\n";

  std::vector<LogicParameter> logic;
  std::vector<OrderParameter> order;
  for ( uint64_t d = 0; d < D; ++ d ) {
    uint64_t n = data_ -> network_ . inputs ( d ) . size ();
    uint64_t m = data_ -> network_ . outputs ( d ) . size ();
    std::string hex_code = data_ -> factors_ [ d ] [ logic_indices[d] ];
    LogicParameter logic_param ( n, m, hex_code );
    OrderParameter order_param ( m, order_indices[d] );
    logic . push_back ( logic_param );
    order . push_back ( order_param );
  }
  Parameter result ( logic, order, data_ -> network_ );
  return result;
}

INLINE_IF_HEADER_ONLY uint64_t ParameterGraph::
index ( Parameter const& p ) const {

  std::vector<LogicParameter> logic = p . logic ( );
  std::vector<OrderParameter> order = p . order ( );

  /// Construct Logic indices
  std::vector<uint64_t> logic_indices;
  uint64_t D = data_ -> network_ . size ();
  for ( uint64_t d = 0; d< D; ++d ) {
      std::string hexcode = logic [ d ] . hex ( );
      /// If factors_ was a different container, we could use find instead
      /// find the hex code within factors_ for the appropriate node
      uint64_t hexsize = data_ -> factors_ [ d ] . size ( );
      for ( uint64_t i = 0; i < hexsize; ++i ) {
          if ( hexcode == data_ -> factors_ [ d ] [ i ] ) {
              logic_indices . push_back ( i );
          }
      }
  }

  /// Construct Order indices
  std::vector<uint64_t> order_indices;
  for ( uint64_t d = 0; d < D; ++ d ) {
      order_indices . push_back ( order[d].index() );
  }


  std::cout << "\n-----------------------\n";
  std::cout << "Method ParameterGraph::Index \n";
  std::cout << "logic indices : ";
  for ( auto i : logic_indices ) std::cout << i << " ";
  std::cout << "\n";
  std::cout << "order indices : ";
  for ( auto i : order_indices ) std::cout << i << " ";
  std::cout << "\n-----------------------\n";


  std::vector<uint64_t> place_values ( 2*D );
  // place_values[0] = 1;
  // for ( uint64_t i = 1; i < 2*D; ++ i ) {
  //   uint64_t possibilities = ( i < D ) ? data_ -> factors_ [ i ] . size () :
  //                                        order [ i - D ] . size ();
  //
  //   place_values[i] = place_values[i-1] * possibilities;
  // }

  place_values[0] = 1;
  place_values[D] = 1;
  std::vector<uint64_t> possibilities ( 2*D );
  for ( uint64_t i = 1; i < D; ++ i ) {
    possibilities [ i ] = data_ -> factors_ [ i - 1 ] . size ();
  }
  for ( uint64_t i = D+1; i < 2*D; ++ i ) {
    possibilities [ i ] = order [ i - D - 1] . size ();
  }
  for ( uint64_t i = 1; i < D; ++ i ) {
    place_values[i] = place_values[i-1] * possibilities[i];
  }
  for ( uint64_t i = D+1; i < 2*D; ++ i ) {
    place_values[i] = place_values[i-1] * possibilities[i];
  }

  std::cout << "\n-----------------------\n";
  std::cout << "Method ParameterGraph::Index \n";
  std::cout << "place values : ";
  for ( auto i : place_values ) std::cout << i << " ";
  std::cout << "\n-----------------------\n";

  uint64_t logic_index = 0;
  for ( uint64_t i = 0; i < D; ++ i ) {
    logic_index += place_values[i] * logic_indices [ i ];
  }
  uint64_t order_index = 0;
  for ( uint64_t i = D; i < 2*D; ++ i ) {
    order_index += place_values[i] * order_indices [ i - D ];
  }


  std::cout << "\n-----------------------\n";
  std::cout << "Method ParameterGraph::Index \n";
  std::cout << "logic index : " << logic_index << "\n";
  std::cout << "order index : " << order_index;
  std::cout << "\n-----------------------\n";


    // uint64_t result = 0;
    // for ( uint64_t i = 0; i < 2*D; ++ i ) {
    //   uint64_t digit = (i < D) ? logic_indices [ i ] : order_indices [ i - D ];
    //   result += place_values[i] * digit;
    // }
    // return result;

  return order_index * data_ -> fixedordersize_ + logic_index;
}

INLINE_IF_HEADER_ONLY std::vector<uint64_t> ParameterGraph::
adjacencies ( uint64_t index ) const {
  throw std::runtime_error ( "Feature not implemented" );   //TODO
}

INLINE_IF_HEADER_ONLY Network const ParameterGraph::
network ( void ) const {
  return data_ -> network_;
}

INLINE_IF_HEADER_ONLY std::ostream& operator << ( std::ostream& stream, ParameterGraph const& pg ) {
  stream <<  "(ParameterGraph: "<< pg.size() << " parameters, "
         << pg.network().size() << " nodes)";
  return stream;
}

INLINE_IF_HEADER_ONLY uint64_t ParameterGraph::
fixedordersize ( void ) const {
  return data_ -> fixedordersize_;
}

INLINE_IF_HEADER_ONLY uint64_t ParameterGraph::
reorderings ( void ) const {
  return data_ -> reorderings_;
}

INLINE_IF_HEADER_ONLY uint64_t ParameterGraph::
_factorial ( uint64_t m ) const {
  static const std::vector<uint64_t> table =
    { 1, 1, 2, 6, 24, 120, 720, 5040, 40320, 362880};
  if ( m < 10 ) return table [ m ]; else return m * _factorial ( m - 1 );
}


#endif
