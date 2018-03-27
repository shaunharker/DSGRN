/// LogicParameter.hpp
/// Shaun Harker
/// 2015-05-24

#pragma once

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

INLINE_IF_HEADER_ONLY uint64_t LogicParameter::
bin ( uint64_t input_combination ) const {
  uint64_t result = 0;
  uint64_t start = input_combination * data_ -> m_;
  uint64_t end = start + data_ -> m_;
  for ( uint64_t bit = start; bit < end; ++ bit ) {
    if ( data_ -> comp_ [ bit ] ) ++ result; else break;
  }
  return result;
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

INLINE_IF_HEADER_ONLY uint64_t LogicParameter::
numInputs() const {
  return data_ -> n_;
}

INLINE_IF_HEADER_ONLY uint64_t LogicParameter::
numOutputs() const {
  return data_ -> m_;
}

INLINE_IF_HEADER_ONLY std::string const& LogicParameter::
hex ( void ) const {
    return data_ -> hex_;
}

INLINE_IF_HEADER_ONLY std::vector<LogicParameter> LogicParameter::
adjacencies ( void ) const {
  std::vector<LogicParameter> output;
  //
  typedef bool bitType;
  typedef std::vector<bitType> BitContainer;
  // Convert an hex char into a vector of bits (length 4)
  // standard order (right to left)
  static std::unordered_map<char, BitContainer> hex_lookup =
    { { '0', {0,0,0,0} }, { '1', {0,0,0,1} }, { '2', {0,0,1,0} },
      { '3', {0,0,1,1} }, { '4', {0,1,0,0} }, { '5', {0,1,0,1} },
      { '6', {0,1,1,0} }, { '7', {0,1,1,1} }, { '8', {1,0,0,0} },
      { '9', {1,0,0,1} }, { 'A', {1,0,1,0} }, { 'B', {1,0,1,1} },
      { 'C', {1,1,0,0} }, { 'D', {1,1,0,1} }, { 'E', {1,1,1,0} },
      { 'F', {1,1,1,1} }
    };
  auto Hex2Bin = [&](char c) {
    return hex_lookup[c];
  };
  //
  // convert a string of hex code into vector of bits.
  auto Hex2BinCode = [&Hex2Bin] ( const std::string & str ) -> BitContainer {
    BitContainer output;
    for ( const char & c : str ) {
      // Need to reverse the order
      BitContainer nybble = Hex2Bin ( c );
      for ( bool b : nybble ) output . push_back ( b );
    }
    return output;
  };
  //
  static const std::vector<char> hex_digit =
  {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
  auto Bin2HexCode = [&] ( const BitContainer & bin ) -> std::string {
    std::string result;
    for ( uint64_t i = 0; i < bin . size (); i += 4 ) {
      short digit = ( (short) bin[i] << 3 ) + ( (short) bin[i+1] << 2 )
                   + ( (short) bin[i+2] << 1 ) + ( (short) bin [i+3] );
      result . push_back ( hex_digit [ digit ] );
    }
    return result;
  };
  //
  /// DEBUG hex/bin conversion
  // std::string testString = "0F9ABD12146C";
  // BitContainer testBool = Hex2BinCode ( testString );
  // for ( bitType b : testBool ) std::cout << b << " ";
  // std::cout << "\n";
  // std::string s = Bin2HexCode ( Hex2BinCode ( testString ) );
  // std::cout << testString << "\n";
  // std::cout << s << "\n";
  /// END DEBUG
  /// Convert the hexCode into binary code
  BitContainer binCode ( Hex2BinCode ( data_ -> hex_ ) );
  /// Flip one bit at a time, construct a new adjacent hex code
  /// and retrieve its index
  uint64_t N = binCode . size ( );
  for ( uint64_t i = 0; i < N; ++i ) {
    binCode[i] = !binCode[i];
    output . push_back ( LogicParameter ( data_ -> n_, data_ -> m_ , Bin2HexCode ( binCode ) ) );
    binCode[i] = !binCode[i];
  }
  return output;
}

INLINE_IF_HEADER_ONLY bool LogicParameter::
operator == ( LogicParameter const& rhs ) const {
  if ( data_ -> hex_ != rhs . data_ -> hex_ ) return false;
  if ( data_ -> n_ != rhs . data_ -> n_ ) return false;
  if ( data_ -> m_ != rhs . data_ -> m_ ) return false;
  return true;
}

INLINE_IF_HEADER_ONLY std::ostream& operator << ( std::ostream& stream, LogicParameter const& p ) {
  stream << p.stringify ();
  return stream;
}
