/// LogicParameter.h
/// Shaun Harker
/// 2015-05-24

#ifndef DSGRN_LOGICPARAMETER_H
#define DSGRN_LOGICPARAMETER_H

#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <cstdlib>

class LogicParameter_ {
public:
  /// assign
  ///   Assign data
  void
  assign ( uint64_t n, uint64_t m, std::string const& hex );

  /// operator ()
  ///   Return true if input combination drives the output
  ///   to the right, and false if it drives it to the left
  bool
  operator () ( std::vector<bool> const& input_combination, uint64_t output ) const;

  /// operator ()
  ///   Version of operator () where bit has already been computed
  bool
  operator () ( uint64_t bit ) const;

  /// stringify
  ///   Return a JSON-style string
  ///    of form [n,m,"hex"]
  std::string
  stringify ( void ) const;

  /// parse
  ///   Initialize from a JSON-style string
  void
  parse ( std::string const& str );

  /// operator <<
  ///   Output debug data to stream
  friend std::ostream& operator << ( std::ostream& stream, LogicParameter_ const& p );

private:
  std::string hex_;
  std::vector<bool> comp_;
  uint64_t n_;
  uint64_t m_;
};

inline void LogicParameter_::
assign ( uint64_t n, uint64_t m, std::string const& hex ) {
  hex_ = hex;
  n_ = n;
  m_ = m;
  uint64_t N = (1 << n_) * m_;
  comp_ . clear ();
  uint64_t L = hex . size ();
  uint64_t count = 0;
  for ( int64_t i = L-1; i >= 0; -- i ) {
    char hex_digit = hex [ i ];
    hex_digit -= '0';
    if ( hex_digit >= 10 ) hex_digit += ('0'-'A'+10);
    comp_ . push_back ( hex_digit & 1 ); 
    comp_ . push_back ( hex_digit & 2 ); 
    comp_ . push_back ( hex_digit & 4 ); 
    comp_ . push_back ( hex_digit & 8 ); 
  }
  comp_ . resize ( N );
}

inline bool LogicParameter_::
operator () ( std::vector<bool> const& input_combination, uint64_t output ) const {
  uint64_t i = 0;
  uint64_t bit = 1;
  for ( bool const& testbit : input_combination ) {
    if ( testbit ) i |= bit;
    bit <<= 1;
  }
  return comp_ [ i*m_ + output ];
}

inline bool LogicParameter_::
operator () ( uint64_t bit ) const {
  return comp_ [ bit ];
}

inline std::string LogicParameter_::
stringify ( void ) const {
  std::stringstream ss;
  ss << "[" << n_ << "," << m_ << ",\"" << hex_ << "\"]";
  return ss . str ();
} 

inline void LogicParameter_::
parse ( std::string const& str ) {
  std::string s = str;
  auto validcharacter = [] (char c) {
    if ( c >= '0' && c <= '9' ) return true;
    if ( c >= 'A' && c <= 'F' ) return true;
    return false;
  };
  for ( char & c : s ) if ( not validcharacter ( c ) ) c = ' ';
  std::stringstream ss ( s );
  ss >> n_;
  ss >> m_;
  ss >> hex_;
  assign ( n_, m_, hex_ );
}


inline std::ostream& operator << ( std::ostream& stream, LogicParameter_ const& p ) {
  stream << p.stringify ();
  return stream;
}

#endif
