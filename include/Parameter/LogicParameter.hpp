/// LogicParameter.hpp
/// Shaun Harker
/// 2015-05-24

#ifndef DSGRN_LOGICPARAMETER_HPP
#define DSGRN_LOGICPARAMETER_HPP

#ifndef INLINE_IF_HEADER_ONLY
#define INLINE_IF_HEADER_ONLY
#endif

#include "LogicParameter.h"

INLINE_IF_HEADER_ONLY LogicParameter::
LogicParameter ( void ) { 
  data_ . reset ( new LogicParameter_ );
}

INLINE_IF_HEADER_ONLY LogicParameter::
LogicParameter ( uint64_t n, uint64_t m, std::string const& hex ) {
  assign ( n, m, hex );
}

INLINE_IF_HEADER_ONLY void LogicParameter::
assign ( uint64_t n, uint64_t m, std::string const& hex ) {
  data_ . reset ( new LogicParameter_ );
  data_ -> hex_ = hex;
  data_ -> n_ = n;
  data_ -> m_ = m;
  uint64_t N = (1 << data_ ->n_) * data_ ->m_;
  data_ -> comp_ . clear ();
  uint64_t L = hex . size ();
  uint64_t count = 0;
  for ( int64_t i = L-1; i >= 0; -- i ) {
    char hex_digit = hex [ i ];
    hex_digit -= '0';
    if ( hex_digit >= 10 ) hex_digit += ('0'-'A'+10);
    data_ -> comp_ . push_back ( hex_digit & 1 ); 
    data_ -> comp_ . push_back ( hex_digit & 2 ); 
    data_ -> comp_ . push_back ( hex_digit & 4 ); 
    data_ -> comp_ . push_back ( hex_digit & 8 ); 
  }
  data_ -> comp_ . resize ( N );
}

INLINE_IF_HEADER_ONLY bool LogicParameter::
operator () ( std::vector<bool> const& input_combination, uint64_t output ) const {
  uint64_t i = 0;
  uint64_t bit = 1;
  for ( bool const& testbit : input_combination ) {
    if ( testbit ) i |= bit;
    bit <<= 1;
  }
  return data_ -> comp_ [  i* data_ -> m_ + output ];
}

INLINE_IF_HEADER_ONLY bool LogicParameter::
operator () ( uint64_t bit ) const {
  return data_ -> comp_ [ bit ];
}

INLINE_IF_HEADER_ONLY std::string LogicParameter::
stringify ( void ) const {
  std::stringstream ss;
  ss << "[" << data_ ->n_ << "," << data_ ->m_ << ",\"" << data_ ->hex_ << "\"]";
  return ss . str ();
} 

INLINE_IF_HEADER_ONLY void LogicParameter::
parse ( std::string const& str ) {
  //std::cout << "LogicParameter::parse(" << str << ")\n";
  std::string s = str;
  auto validcharacter = [] (char c) {
    if ( c >= '0' && c <= '9' ) return true;
    if ( c >= 'A' && c <= 'F' ) return true;
    return false;
  };
  for ( char & c : s ) if ( not validcharacter ( c ) ) c = ' ';
  std::stringstream ss ( s );
  uint64_t n, m;
  std::string hex;
  ss >> n;
  ss >> m;
  ss >> hex;
  //std::cout << "  Parsed (" << n << ", " << m << ", " << hex << ")\n";
  assign ( n, m, hex );
}


INLINE_IF_HEADER_ONLY std::ostream& operator << ( std::ostream& stream, LogicParameter const& p ) {
  stream << p.stringify ();
  return stream;
}

#endif
