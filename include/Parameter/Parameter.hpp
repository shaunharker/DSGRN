/// Parameter.hpp
/// Shaun Harker
/// 2015-05-24

#ifndef DSGRN_PARAMETER_HPP
#define DSGRN_PARAMETER_HPP

#ifndef INLINE_IF_HEADER_ONLY
#define INLINE_IF_HEADER_ONLY
#endif

#include "Parameter.h"

INLINE_IF_HEADER_ONLY Parameter::
Parameter ( void ) {
  data_ . reset ( new Parameter_ );
}

INLINE_IF_HEADER_ONLY Parameter::
Parameter ( std::vector<LogicParameter> const& logic,
            std::vector<OrderParameter> const& order,
            Network const& network ) {
  assign ( logic, order, network );
}

INLINE_IF_HEADER_ONLY Parameter::
Parameter ( Network const& network ) {
  assign ( network );
}

INLINE_IF_HEADER_ONLY void Parameter::
assign ( std::vector<LogicParameter> const& logic,
         std::vector<OrderParameter> const& order,
         Network const& network ) {
  data_ . reset ( new Parameter_ );
  data_ -> logic_ = logic;
  data_ -> order_ = order;
  data_ -> network_ = network;
}

INLINE_IF_HEADER_ONLY void Parameter::
assign ( Network const& network ) {
  data_ . reset ( new Parameter_ );
  data_ -> network_ = network;
}


INLINE_IF_HEADER_ONLY bool Parameter::
attracting ( Domain const& dom ) const {
  int D = data_ -> network_ . size ();
  for ( int d = 0; d < D; ++ d ) {
    if ( not dom . isMin(d) && absorbing ( dom, d, -1 ) ) return false;
    if ( not dom . isMax(d) && absorbing ( dom, d, 1 ) ) return false;
  }
  return true;
}

INLINE_IF_HEADER_ONLY bool Parameter::
absorbing ( Domain const& dom, int collapse_dim, int direction ) const {
  //std::cout << "Absorbing (" << dom . index () << ", " << collapse_dim << ", " << direction << ")\n";
  int thres = dom [ collapse_dim ];
  if ( direction == -1 ) thres -= 1;
  //std::cout << "  Threshold # = " << thres << "\n";
  std::vector<bool> input_combination;
  //std::cout << "  Forming input combination by analyzing inputs of node " << collapse_dim << ".\n";
  for ( int source : data_ -> network_ . inputs ( collapse_dim ) ) {
    //std::cout << "    Analyze source edge " << source << "\n";
    bool activating = data_ -> network_ . interaction ( source, collapse_dim );
    //std::cout << "      This edge is " << (activating ? "activating" : "repressing" ) << ".\n";
    int inedge = data_ -> network_ . order ( source, collapse_dim );
    //std::cout << "      This edge is the " << inedge << "th ordered outedge of " << source << ".\n";
    int thres = data_ -> order_ [ source ] . inverse ( inedge );
    //std::cout << "      The input combination digit depends on which side of threshold " << thres << " on dimension " << source << " we are at.\n";
    bool result = not ( dom [ source ] > thres ) ^ activating;
    //std::cout << "      The domain is on the " << ( ( dom [ source ] > thres ) ? "right" : "left" ) << " side of this threshold.\n";
    input_combination . push_back ( result );
    //std::cout << "      Hence, the input combination digit is " << (result ? "1" : "0") << "\n";
  }
  //std::cout << "  Input combination formed. Big-endian representation = ";
  //for ( int i = input_combination . size () - 1; i >= 0; -- i ){
  // std::cout << (input_combination[i] ? "1" : "0");
  //}
  //std::cout << "\n";
  //std::cout << "  Consulting parameter " <<  data_ -> logic_ [ collapse_dim ] . stringify () << ".\n";
  bool flow_direction = data_ -> logic_ [ collapse_dim ] ( input_combination, thres );
  //std::cout << "  Flow direction is to the " << (flow_direction ? "right" : "left") << "\n";
  if ( direction == -1 ) {
    //std::cout << "  Hence the left wall is " << ((flow_direction)?"not ":"") << "absorbing.\n";
    return not flow_direction;
  } else {
    //std::cout << "  Hence the right wall is " << ((not flow_direction)?"not ":"") << "absorbing.\n";
    return flow_direction;
  }
}

INLINE_IF_HEADER_ONLY uint64_t Parameter::
regulator ( uint64_t variable, uint64_t threshold ) const {
  // std::cout << "Parameter::regulator\n";
  // std::cout << "  variable = " << variable << " and threshold = " << threshold << "\n";
  // std::cout << "  returning " << network() . outputs ( variable ) [ threshold ] << "\n";
  int inedge = order()[variable](threshold);
  return network() . outputs ( variable ) [ inedge ];
}

INLINE_IF_HEADER_ONLY std::vector<uint64_t> Parameter::
labelling ( void ) const {
  std::vector<uint64_t> result;
  uint64_t D = network () . size ();

  // pre-allocated vectors (for efficiency)
  std::vector<uint64_t> lower_limits ( D );
  std::vector<uint64_t> upper_limits ( D );
  std::vector<uint64_t> dom ( D );
  std::vector<uint64_t> width ( D );

  std::vector<uint64_t> limits = network () . domains ();
  std::vector<uint64_t> jump ( D ); // index offset in each dim
  uint64_t N = 1;
  for ( uint64_t d = 0; d < D; ++ d ) {
    jump[d] =  N;
    N *= limits [ d ];
  }
  // N is now number of domains
  // Domains are implicitly indexed.
  // "jump" is an array telling us how much to change the index
  //   to move +1 in each dimension
  result . resize ( N, 0 );
  for ( int d = 0; d < D; ++ d ) {
    uint64_t n = network() . inputs ( d ) . size ();
    uint64_t numInComb = ( 1LL << n );
    //uint64_t num_debug_domains = 0;
    for ( uint64_t in = 0; in < numInComb; ++ in ) {
      /// What bin does the target point land in for dimension d?
      uint64_t bin = data_ -> logic_ [ d ] . bin ( in );
      /// Which domains have this input combination for dimension d?
      std::fill ( lower_limits.begin(), lower_limits.end(), 0 );
      upper_limits = limits;
      uint64_t sources = network () . inputs ( d ) . size ();
      for ( uint64_t inorder = 0; inorder < sources; ++ inorder ) {
        //std::cout << "Dim " << d << ", in = " << in << " inorder = " << inorder << "\n";
        uint64_t source = network () . inputs ( d ) [ inorder ];
        //std::cout << "source = " << source << "\n";
        bool activating = network () . interaction ( source, d );
        //std::cout << "high is activating? " << ( activating ? "yes" : "no" ) << "\n";
        int outorder = network () . order ( source, d );
        //std::cout << "outorder = " << outorder << "\n";
        bool side = in & ( 1LL << inorder );
        //std::cout << "on activating side? " << ( side ? "yes" : "no" ) << "\n";
        uint64_t thres = data_ -> order_ [ source ] . inverse ( outorder ) + 1;
        //std::cout << "critical bin = " << thres << "\n";
        if ( activating ^ side ) {
          //std::cout << "Case A.\n";
          lower_limits[source] = 0;
          upper_limits[source] = thres;
        } else {
          //std::cout << "Case B.\n";
          lower_limits[source] = thres;
          upper_limits[source] = limits [ source ];
        }
      }
      /// Iterate through two zones:
      ///   Zone 1. domain left of bin
      ///   Zone 2. domain right of bin
      ///   Note. domains matching bin do not
      ///         require anything to be done
      auto apply_mask = [&] ( uint64_t mask ) {
        // Iterate between lower and upper limits applying mask
        uint64_t dom_index = 0;
        dom = lower_limits;
        for ( uint64_t k = 0; k < D; ++ k ) {
          width[k] = upper_limits[k] - lower_limits[k];
          dom_index += jump[k] * lower_limits[k];
          if ( width[k] == 0 ) return;
        }
        while ( 1 ) {
          // apply mask
          result [ dom_index ] |= mask;
          // next domain
          for ( uint64_t k = 0; k < D; ++ k ) {
            ++ dom[k];
            dom_index += jump[k];
            if ( dom[k] < upper_limits[k] ) break;
            dom[k] = lower_limits[k];
            dom_index -= width[k] * jump[k];
          }
          // If we are back to start, return
          bool done = true;
          for ( uint64_t k = 0; k < D; ++ k ) {
            if ( dom[k] != lower_limits[k] ) done = false;
          }
          if ( done ) break;
        }
      };

      uint64_t left = lower_limits [ d ];
      uint64_t right = upper_limits [ d ];

      // Zone 1. (Flows to right.)
      if ( bin > left ) {
        lower_limits [ d ] = left;
        upper_limits [ d ] = bin;
        apply_mask (1LL << (D+d));
      }
      // Zone 2. (Flows to left.)
      if ( bin+1 < right ) {
        lower_limits [ d ] = bin + 1;
        upper_limits [ d ] = right;
        apply_mask (1LL << d);
      }
    }
  }
  return result;
}

INLINE_IF_HEADER_ONLY Network const Parameter::
network ( void ) const {
  return data_ -> network_;
}

INLINE_IF_HEADER_ONLY std::string Parameter::
stringify ( void ) const {
  std::stringstream ss;
  uint64_t D = data_ -> network_ . size ();
  ss << "[";
  for ( uint64_t d = 0; d < D; ++ d ) {
    if ( d > 0 ) ss << ",";
    ss << "[\"" << network() . name ( d ) << "\","
       << data_ -> logic_[d] << "," << data_ -> order_[d] << "]";
  }
  ss << "]";
  return ss . str ();
}

INLINE_IF_HEADER_ONLY void Parameter::
parse ( std::string const& str ) {
  json p = json::parse(str);
  data_ -> logic_ . clear ();
  data_ -> order_ . clear ();
  for ( auto network_node : p ) {
    data_ -> logic_ . push_back ( LogicParameter () );
    data_ -> order_ . push_back ( OrderParameter () );
    // (*node)[0] has node name, which we ignore.
    data_ -> logic_ . back() . parse ( json::stringify ( network_node[1] )); //TODO inefficient
    data_ -> order_ . back() . parse ( json::stringify ( network_node[2] )); //TODO inefficient
  }
}

INLINE_IF_HEADER_ONLY std::string Parameter::
inequalities ( void ) const {
  // input_string
  //   Given an input edge i of a node d output the L/U indexing associated
  auto input_string = [&](uint64_t i, uint64_t d ) {
    uint64_t source = network() . inputs ( d ) [ i ];
    std::string const& node_name = network() . name ( d );
    std::string source_name = network() . name(source);
    std::stringstream input_ss;
    input_ss << "[" << source_name << "," << node_name << "]";
    return input_ss . str ();
  };
  // output_string
  //   Given an output edge j, output the THETA variable associated with it
  auto output_string = [&](uint64_t j, uint64_t d ) {
    uint64_t target = network() . outputs ( d ) [ data_ -> order_[d](j) ];
    std::string const& node_name = network() . name ( d );
    std::string target_name = network() . name(target);
    std::stringstream output_ss;
    output_ss << "T[" << node_name << "," << target_name << "]";
    return output_ss . str ();
  };
  // input_combo_string
  //   Given an input combination i, return the algebraic formula
  //   of U's and L's associated with that input combination.
  auto input_combo_string = [&](uint64_t i, uint64_t d) {
    std::stringstream input_ss;
    std::vector<std::vector<uint64_t>> logic = 
      network () . logic ( d );
    std::string const& node_name = network() . name ( d );
    uint64_t n = network() . inputs ( d ) . size ();
    uint64_t bit = 1;
    uint64_t k = 0;
    for ( auto const& factor : logic ) {
      if ( factor . size () > 1 ) input_ss << "(";
      bool inner_first = true;
      for ( uint64_t source : factor ) {
        if ( inner_first ) inner_first = false; else input_ss << " + ";
        std::string source_name = network() . name(source);
        if ( i & bit ) {
          input_ss << "U[" << source_name <<"," << node_name << "]";
        } else {
          input_ss << "L[" << source_name <<"," << node_name << "]";
        }
        bit <<= 1;
        ++ k;
      }
      if ( factor . size () > 1 ) input_ss << ")"; 
      else if ( k < n ) input_ss << " ";
    }
    return input_ss . str ();
  };


  std::stringstream ss;
  uint64_t D = data_ -> network_ . size ();
  ss << "{ \"inequalities\" : \"";

  bool outerfirst = true;
  for ( uint64_t d = 0; d < D; ++ d ) {
    if ( outerfirst ) outerfirst = false; else ss << " && ";
    uint64_t n = network() . inputs ( d ) . size ();
    uint64_t m = network() . outputs ( d ) . size ();
    uint64_t N = ( 1LL << n );
    // Output all inequalities comparing input formulas to thresholds
    bool first = true;
    for ( uint64_t i = 0; i < N; ++ i ) {
      if ( first ) first = false; else ss << " && ";
      uint64_t j = 0;
      while ( j < m && data_ -> logic_[d] ( m*i + j ) ) ++ j;
      if ( j == 0 ) {
        ss << input_combo_string ( i, d ) << " < " << output_string ( 0, d );
      } else if ( j == m ) {
        ss << output_string ( m-1, d ) << " < " << input_combo_string ( i, d );
      } else {
        ss << output_string ( j-1, d ) << " < " 
           << input_combo_string ( i, d ) << " < " 
           << output_string ( j, d );
      }
    }
    ss << " && ";
    // Output all inequalities comparing thresholds
    first = true;
    for ( uint64_t j = 0; j < m; ++ j ) {
      if ( first ) { 
        first = false;
        ss << "0 < ";
      } else { 
        ss << " < ";
      }
      ss << output_string ( j, d );
    }
    ss << " && ";
    // Output 0 < L < U  constraints
    first = true;
    for ( uint64_t i = 0; i < n; ++ i ) {
      if ( first ) first = false; else ss << " && ";
      ss << "0 < L" << input_string ( i, d ) << " < U" << input_string ( i, d );
    }

  }
  ss << "\", \"variables\" : \"{";
  // Output variable list
  outerfirst = true;
  for ( uint64_t d = 0; d < D; ++ d ) {
    uint64_t n = network() . inputs ( d ) . size ();
    uint64_t m = network() . outputs ( d ) . size ();
    if ( outerfirst ) outerfirst = false; else ss << ", ";
    for ( uint64_t i = 0; i < n; ++ i ) {
      ss << "L" << input_string ( i, d ) << ", ";
    }
    for ( uint64_t i = 0; i < n; ++ i ) {
      ss << "U" << input_string ( i, d ) << ", ";
    }
    bool first = true;
    for ( uint64_t j = 0; j < m; ++ j ) {
      if ( first ) first = false; else ss << ", ";
      ss << output_string ( j, d );
    }
  }
  ss << "}\"}";
  return ss . str ();
}

INLINE_IF_HEADER_ONLY std::vector<LogicParameter> const & Parameter::
logic ( void ) const {
    return data_ -> logic_;
}

INLINE_IF_HEADER_ONLY std::vector<OrderParameter> const & Parameter::
order ( void ) const {
    return data_ -> order_;
}

INLINE_IF_HEADER_ONLY std::ostream& operator << ( std::ostream& stream, Parameter const& p ) {
  stream << p.stringify();
  return stream;
}

#endif
