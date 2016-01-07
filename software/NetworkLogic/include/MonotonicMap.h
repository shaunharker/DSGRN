/// MonotonicMap.h
/// Author: Shaun Harker
/// Date: August 20, 2014

#ifndef MONOTONICMAP_H
#define MONOTONICMAP_H

#include <iostream>
#include <vector>
#include "boost/foreach.hpp"
#include <memory>

/// class MonotonicMap
/// a "smart vertex" class representing the dynamics of a node of
/// a boolean switching network
class MonotonicMap {
public:
  // data
  int64_t n; // number of in edges  -- domain is {0,1,...,2^n-1}
  int64_t m; // number of out edges -- codomain is {0,1,...,m}

  /// logic_
  ///    An expression of the form (a+b+c)(d+e)f(g+h)
  ///    would be encoded as vector of ints 3,2,1,2 (the number of summands in each
  ///    factor). Up-regulation and down-regulation are of no concern here (they are
  ///    treated elsewhere)
  ///    The entries in logic_ line up to the bits in the domain of bin_
  ///    in such a way that the last entry of logic_ corresponds to the least
  ///    significant bit of bin_'s domain.
  std::vector< int64_t> logic_; // logic
  
  /// bin_
  ///    bin_ is an array storing the values of
  ///    a map from {0,1,...2^n-1} -> {0,1,...m}
  std::vector< int64_t > bin_;  // mapping

  /// constraints_
  ///    constraints_ is an array storing value of the form (m, (x,y)),
  ///    where m, x, and y are int64_t.
  ///    The effect of an item in constraints_ is to enforce extra
  ///    realizability conditions
  ///    bin_[a] < bin_[b]  whenever (a & ~m ) == (b & ~m)  and (a&m == x) and (b&m == y)
  std::vector<std::pair<int64_t,std::pair<int64_t, int64_t>>> constraints_;

  // constructors
  MonotonicMap ( void ) {}
  MonotonicMap ( int64_t n, int64_t m ) : n(n), m(m) {
    bin_ . resize ( (1 << n), 0 );
    logic_ . resize ( 1, n ); // all-sum by default
  }
  MonotonicMap ( int64_t n, int64_t m, std::vector<int64_t> const& logic ): n(n), m(m), logic_(logic) {
    bin_ . resize ( (1 << n), 0 );
  }
  MonotonicMap ( int64_t n, int64_t m, 
                 std::vector<int64_t> const& logic, 
                 std::vector<std::pair<int64_t,std::pair<int64_t,int64_t>>> const& constraints )
  : n(n), m(m), logic_(logic), constraints_(constraints) {
    bin_ . resize ( (1 << n), 0 );
  }
  MonotonicMap ( int64_t n, 
                 int64_t m, 
                 std::vector<int64_t> const& logic, 
                 std::vector<std::pair<int64_t,std::pair<int64_t,int64_t>>> const& constraints,
                 std::vector<int64_t> const& data )
    : n(n), m(m), logic_(logic), constraints_(constraints), bin_(data) {}

  // Check if monotonic
  bool monotonic ( void ) const {
    //std::cout << "Calling monotonic\n";
    int64_t N = (1 << n);
    for ( int64_t i = 0; i < N; ++ i ) {
      std::vector<int64_t> children;
      for ( int64_t pos = 0; pos < n; ++ pos ) {
        int64_t bit = 1 << pos;
        if ( not ( i & bit ) ) { 
          //std::cout << "Pushing child " << (i|bit) << " of " << i << "\n";
          children . push_back ( i | bit );
        }
      }
      BOOST_FOREACH ( int64_t child, children ) {
        if ( bin_[child] < bin_[i] ) { 
          //std::cout << " Return false because bin_[" << child << "] < bin_[" << i << "]\n";
          //std::cout << " bin_[" << child << "] = " << bin_[child] << "\n";
          //std::cout << " bin_[" << i << "] = " << bin_[i] << "\n";

          return false;
        }
      }
    }
    //std::cout << "Returning true.\n";
    return true;
  }

  bool realizable ( void ) const {
    // Step 1. Check constraints. (current algorithm takes 2^{2n}*|constraints| time) 
    {
      int64_t N = (1 << n);
      for ( int64_t a = 0; a < N; ++ a ) {
        for ( int64_t b = 0; b < N; ++ b ) {
          for ( std::pair<int64_t, std::pair<int64_t, int64_t>> const& constraint : constraints_ ) {
            int64_t const& mask = constraint . first;
            int64_t const& x = constraint . second . first;
            int64_t const& y = constraint . second . second;
            if ( ((a & ~mask) == (b & ~mask)) && ( (a & mask) == x ) && ( (b & mask) == y ) ) {
              if ( bin_[a] > bin_[b] ) return false;
            }
          }
        }
      }
    }
    // Step 2. Other realizability conditions
    int64_t max_terms_in_factor = 0;
    for ( int64_t i = 0; i < logic_ . size (); ++ i ) {
      max_terms_in_factor = std::max ( max_terms_in_factor, logic_[i] );
    }

    if ( (logic_ . size () == 1) || (max_terms_in_factor == 1) ) {
      // Case (n) (all sum case) or Case (1,1,1,1...,1) (n-times, all product case)
      int64_t N = (1 << n);
      for ( int64_t i = 0; i < N; ++ i ) {
        // I corresponds to the bits of i that are on.
        //std::cout << "Top of loop\n";

        for ( int64_t a = 0; a < N; ++ a ) {
          if ( (a & i) != a ) continue;
          for ( int64_t b = 0; b < N; ++ b ) {
            if ( (b & i) != b ) continue;
            bool less = false;
            bool greater = false;
            for ( int64_t c = 0; c < N; ++ c ) {
              if ( (c & i ) != 0 ) continue;
              int64_t x = bin_[a|c];
              int64_t y = bin_[b|c];
              if ( x < y ) { 
                less = true;
              }
              if ( x > y ) { 
                greater = true;
              }
              if ( less && greater ) {
                //std::cout << "Returning false.\n";
                return false;
              }
            }
          }
        }
      }
      return true;
    } else if ( logic_ . size () == 2 ) {
      if ( logic_[0] == 2 && logic_[1] == 1 ) {
        // Case (2, 1)  (a+b)c
        // The a,b,c is encoded bitwise as cba (i.e. "a" is least sig. bit)
        // In this notation the rules for sum-product are
        // " 010 < 100 implies 011 <= 101 "  (Rule A)
        // " 001 < 100 implies 011 <= 110 "  (Rule B)
        // " 010 > 001 implies 110 >= 101 "  (Rule C)
        // " 010 < 001 implies 110 <= 101 "  (Rule C, reversed)
        int64_t D001 = bin_[1];
        int64_t D010 = bin_[2];
        int64_t D011 = bin_[3];
        int64_t D100 = bin_[4];
        int64_t D101 = bin_[5];
        int64_t D110 = bin_[6]; 

        if ( (D010 < D100) && not (D011 <= D101) ) return false;
        if ( (D001 < D100) && not (D011 <= D110) ) return false;
        if ( (D010 > D001) && not (D110 >= D101) ) return false;
        if ( (D010 < D001) && not (D110 <= D101) ) return false;
        return true;
      }
      if ( logic_[0] == 1 && logic_[1] == 2 ) {
        // Case (1,2). Symmetric to case (2,1). (We just rotate the bits)
        int64_t D001 = bin_[1];
        int64_t D010 = bin_[2];
        int64_t D011 = bin_[3];
        int64_t D100 = bin_[4];
        int64_t D101 = bin_[5];
        int64_t D110 = bin_[6]; 

        if ( (D100 < D001) && not (D110 <= D011) ) return false;
        if ( (D010 < D001) && not (D110 <= D101) ) return false;
        if ( (D100 > D010) && not (D101 >= D011) ) return false;
        if ( (D100 < D010) && not (D101 <= D011) ) return false;
        return true;     
      }
      if ( logic_[0] == 2 && logic_[1] == 2 ) {
        // Case (2,2). "(a+b)(c+d)"
        // Slice Conditions.
        std::vector<int64_t> slices = { 0b1100, 0b0011, 0b1011, 0b0111, 0b1101, 0b1110 };
        for ( int64_t slice : slices ) {
          for ( int64_t x = 0; x < 16; ++ x ) {
            for ( int64_t v = 0; v < 16; ++ v ) {
              int64_t y = (x & slice) | (v & ~slice);
              int64_t u = (x & ~slice) | (v & slice);
              if ( bin_[x] < bin_[y] && !(bin_[u] <= bin_[v]) ) return false;
            }
          }
        }
        // Promotion Condition.
        std::vector<int64_t> factorslices = { 0b1100, 0b0011 };
        for ( int64_t x = 0; x < 16; ++ x ) {
          for ( int64_t y = 0; y < 16; ++ y ) {
            if ( bin_[x] >= bin_[y] ) continue;
            for ( int64_t slice : factorslices ) {
              // Guarantee that f_{slice}(x) >= f_{slice}(y)
              // or else continue
              bool condition_met = false;
              for ( int64_t z = 0; z < 16; ++ z ) {
                if ( bin_ [ (x & slice) | (z & ~slice) ] > bin_ [ (y & slice) | (z & ~slice) ]) {
                  condition_met = true;
                  break;
                }
              }
              if ( not condition_met ) continue;
              // Check all valid promotions and enforce f(X) <= f(Y)
              for ( int i = 0; i < 4; ++ i ) {
                int64_t bit = 1 << i;
                if ( not (slice & bit) ) continue;
                if ( (x & bit) | (y & bit) ) continue;
                int64_t X = x | bit;
                int64_t Y = y | bit;
                if ( bin_[X] > bin_[Y] ) return false;
              }
            }
          }
        }
        return true;
      }
    } 
    //std::cout << "BooleanSwitching Node realizability condition unknown.\n";
    //std::cout << "Logic size = " << logic_ . size () << "\n";
    //for ( int64_t i = 0; i < logic_ . size (); ++ i ) std::cout << logic_[i] << " ";
    //std::cout << "\n";
    throw std::logic_error ( "MonotonicMap:: realizability algorithm cannot handle current situation\n");
    return false;
  }

bool essential ( void ) const {
  //std::cout << "essential line " << __LINE__ << "\n";
  int64_t N = (1 << n);
  // Check that all 0's give 0
  if ( bin_[0] > 0 ) return false;
  // Check that all 1's gives m
  if ( bin_[N-1] < m ) return false;
  // Check for each dimension that there is a switch that matters
  std::vector<bool> checks ( n, false );
  int64_t mask = N - 1;
  for ( int d = 0; d < n; ++ d ) {
    int64_t bit = 1 << d;
    int64_t maskbit = mask ^ bit;
    for ( int64_t i = 0; i < N; ++ i ) {
      if ( bin_[i & maskbit] < bin_[i | bit] ) {
        //std::cout << "dim " << d << " is essential since \n";
        //std::cout << "bin_[" << (i & maskbit) << "] = " << bin_[i & maskbit] << "\n";
        //std::cout << "bin_[" << (i | bit) << "] = " << bin_[i | bit] << "\n";
        checks[d] = true;
        break;
      }
    }
    if ( checks[d] == false ) return false;
  }
  return true;
}

  // return adjacent monotonic maps
  std::vector<std::shared_ptr<MonotonicMap> > neighbors ( void ) const {
    // DEBUG
    //std::cout << "Calling neighbors of: \n";
    //for ( int64_t j = 0; j < (1 << n); ++ j ) {
    //  std::cout << bin_[j] << " ";
    //}
    //std::cout << "\n";
    // END DEBUG

    std::vector<std::shared_ptr<MonotonicMap> > results;

    // Obtain neighbors via changing the monotone function
    std::vector<int64_t> copy = bin_;
    int64_t N = (1 << n);
    for ( int64_t i = 0; i < N; ++ i ) {
      if ( copy[i] > 0 ) {
        -- copy[i];
        std::shared_ptr<MonotonicMap> new_map ( new MonotonicMap ( n, m, logic_, constraints_, copy ) );
        if ( new_map -> monotonic () && new_map -> realizable () ) {
          results . push_back ( new_map );
          //std::cout << "Found neighbor: ";
          //for ( int64_t j = 0; j < N; ++ j ) {
          //  std::cout << copy[j] << " ";
          //}
          //std::cout << "\n";
        }
        ++ copy[i];
      }
      if ( copy[i] < m ) {
        ++ copy[i];
        std::shared_ptr<MonotonicMap> new_map ( new MonotonicMap ( n, m, logic_, constraints_, copy ) );
        if ( new_map -> monotonic () && new_map -> realizable () ) {
          results . push_back ( new_map );
          //std::cout << "Found neighbor: ";
          //for ( int64_t j = 0; j < N; ++ j ) {
          //  std::cout << copy[j] << " ";
          //}
          //std::cout << "\n";          
        }
        -- copy[i];
      }
    }

    return results;
  }

  bool operator == ( const MonotonicMap & rhs ) const {
    if ( n != rhs . n ) return false;
    if ( m != rhs . m ) return false;
    int64_t N = (1 << n);
    for ( int64_t i = 0; i < N; ++ i ) {
      if ( bin_[i] != rhs.bin_[i] ) return false;
    }
    return true;
  }

  /// hex 
  ///   Return a hex code X which represents the parameter. The 
  ///   hex code represents a binary string in big-endian fashion. 
  ///   For 0 <= i < 2^n and 0 <= j < m, let b_{ij} be the
  ///   the (i*m + j)th binary digit of X (where the 0th digit is
  ///   the least significant).
  ///   Then b_{ij} = 1 if input i activates output j
  ///        b_{ij} = 0    otherwise
  std::string hex ( void ) const {
    std::string X;
    int64_t N = ( 1 << n );
    char nybble = 0, mask = 1;
    auto flush_nybble = [&] () { 
      // Hex digits 0-9
      if ( nybble < 10 ) X.push_back((char)(nybble + '0'));
      // Hex digits A-F
      else X.push_back((char)(nybble - 10 + 'A'));
      nybble = 0; mask = 1;
    };
    for ( int64_t i = 0; i < N; ++ i ) {
      for ( int64_t j = 0; j < m; ++ j ) {
        if ( bin_[i] > j ) nybble |= mask;
        mask <<= 1; if ( mask == 16 ) flush_nybble();
      }
    }
    if ( mask != 1 ) flush_nybble ();
    // Put into big-endian form.
    std::reverse( X . begin(), X . end () );
    return X;
  }

  /// prettyPrint
  ///   return string corresponding to monotonic map
  std::string prettyPrint ( std::string const& symbol,
                            std::vector<std::string> const& input_symbols,
                            std::vector<std::string> const& output_symbols ) const {
    if ( input_symbols . size () != n ) {
      std::cout << "input_symbols.size() = " << input_symbols . size () << " != " << n << " = n \n";
      for ( std::string const& s : input_symbols ) std::cout << s << " "; std::cout << "\n";
      std::cout << "output_symbols.size() = " << output_symbols . size () << " != " << m << " = m \n";
      for ( std::string const& s : output_symbols ) std::cout << s << " "; std::cout << "\n";
      for ( int64_t j = 0; j < logic_ . size (); ++ j ) {
        std::cout << logic_[j] << " ";
      }
      std::cout << "\n";
      throw std::logic_error ( "MontonicMap::prettyPrint. input_symbols.size() != n\n");
    }
    if ( output_symbols . size () != m ) {
      std::cout << "output_symbols.size() = " << output_symbols . size () << " != " << m << " = m \n";
      throw std::logic_error ( "MontonicMap::prettyPrint. output_symbols.size() != m\n");
    }
    std::stringstream ss;
    int64_t N = (1 << n);
    for ( int64_t i = 0; i < N; ++ i ) { 
      int64_t bin = bin_[i];
      if ( bin > 0 ) {
        ss << "THETA(" << symbol << ", " << output_symbols[bin-1] << ") <= "; 
      }
      int64_t count = 0;
      for ( int64_t j = 0; j < logic_ . size (); ++ j ) {
        ss << "(";
        for ( int64_t k = 0; k < logic_[j]; ++ k ) {
          if ( (i & ( 1 << count )) == 0 ) {
            ss << "L(";
          } else {
            ss << "U(";
          }
          ss << input_symbols [ count ++ ] << ", " << symbol << ")";
          if ( k != logic_[j]-1 ) ss << " + ";
        }
        ss << ")";
      }
      if ( bin < m ) {
        ss << " <= THETA(" << symbol << ", " << output_symbols[bin] << ")"; 
      }
      ss << ";\n";
    }
    return ss . str ();
  }

  friend std::size_t hash_value ( const MonotonicMap & p ) {
    std::size_t seed = 0;
    int64_t N = (1 << p.n);
    for ( int64_t i = 0; i < N; ++ i ) {
      boost::hash_combine(seed, p.bin_[i] );
    }
    return seed;
  }

  friend std::ostream & operator << ( std::ostream & stream, 
                                      const MonotonicMap & print_me ) {
    stream << print_me.hex ();
    /*
    stream << "{(In,Out)=(" << print_me . n << ", " << print_me . m << "), Logic=(";
    for ( int64_t i = 0; i < print_me . logic_ . size (); ++ i ) { 
      if ( i != 0 ) stream << ",";
      stream << print_me . logic_[i];
    }
    stream << "), Data=(";
    for ( int64_t i = 0; i < print_me . bin_ . size (); ++ i ) { 
      if ( i != 0 ) stream << ",";
      stream << print_me . bin_[i];
    }
    stream << ")}";
    */
    return stream;
  }
};

#endif
