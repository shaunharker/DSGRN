/// DomainGraph.hpp
/// Shaun Harker
/// 2015-05-24

#ifndef DSGRN_DOMAINGRAPH_HPP
#define DSGRN_DOMAINGRAPH_HPP

#ifndef INLINE_IF_HEADER_ONLY
#define INLINE_IF_HEADER_ONLY
#endif

#include "DomainGraph.h"

INLINE_IF_HEADER_ONLY DomainGraph::
DomainGraph ( void ) {
  data_ . reset ( new DomainGraph_ );
}

INLINE_IF_HEADER_ONLY DomainGraph::
DomainGraph ( Parameter const parameter ) {
  assign ( parameter );
}

INLINE_IF_HEADER_ONLY void DomainGraph::
assign ( Parameter const parameter ) {
  data_ . reset ( new DomainGraph_ );
  data_ -> parameter_ = parameter;
  uint64_t D = parameter . network () . size ();
  std::vector<uint64_t> limits = parameter . network() . domains ();
  std::vector<uint64_t> jump ( D ); // index offset in each dim
  uint64_t N = 1;
  for ( uint64_t d = 0; d < D; ++ d ) {
    jump[d] =  N;
    N *=  limits [ d ];
  }
  data_ -> digraph_ = Digraph ();
  data_ -> digraph_ . resize ( N );
  std::vector<uint64_t> labelling = parameter . labelling ();
  for ( uint64_t i = 0; i < N; ++ i ) {
    if ( labelling [ i ] == 0 ) {
      data_ -> digraph_ . add_edge ( i, i );
    }
    uint64_t leftbit = 1;
    uint64_t rightbit = (1LL << D);
    for ( int d = 0; d < D; ++ d, leftbit <<= 1, rightbit <<= 1 ) {
      if ( labelling [ i ] & rightbit ) {
        uint64_t j = i + jump[d];
        if ( not (labelling [ j ] & leftbit) ) {
          data_ -> digraph_ . add_edge ( i, j );
        }
      }
      if ( labelling [ i ] & leftbit ) {
        uint64_t j = i - jump[d];
        if ( not (labelling [ j ] & rightbit) ) {
          data_ -> digraph_ . add_edge ( i, j );
        }
      }
    }
  }
}

INLINE_IF_HEADER_ONLY Digraph const DomainGraph::
digraph ( void ) const {
  return data_ -> digraph_;
}

INLINE_IF_HEADER_ONLY Annotation const DomainGraph::
annotate ( Component const& vertices ) const {
  uint64_t D = data_ ->parameter_ . network() . size ();
  std::vector<uint64_t> limits = data_ -> parameter_ . network() . domains ();
  std::vector<uint64_t> domain_indices ( vertices.begin(), vertices.end() );
  std::vector<uint64_t> min_pos(D);
  std::vector<uint64_t> max_pos(D);
  for ( int d = 0; d < D; ++ d ) {
    min_pos[d] = limits[d];
    max_pos[d] = 0;
  }
  for ( int d = 0; d < D; ++ d ) {
    for ( uint64_t & v : domain_indices ) {
      uint64_t pos = v % limits[d];
      v = v / limits[d];
      min_pos[d] = std::min(min_pos[d], pos);
      max_pos[d] = std::max(max_pos[d], pos);
    }
  }
  std::vector<uint64_t> signature;
  for ( int d = 0; d < D; ++ d ) {
    if ( min_pos[d] != max_pos[d] ) {
      signature . push_back ( d );
    }
  }
  Annotation a;
  std::stringstream ss;
  if ( signature . size () == 0 ) {
    ss << "FP";
    bool all_on = true;
    bool all_off = true;
    for ( int d = 0; d < D; ++ d ) {
      if ( min_pos[d] == 0 ) all_on = false;
      else all_off = false;
    }
    if ( all_on ) ss << " ON";
    if ( all_off) ss << " OFF";
  } else if ( signature . size () == D ) {
    ss << "FC";
  } else {
    ss << "XC {";
    bool first_term = true;
    for ( uint64_t d : signature ) {
      if ( first_term ) first_term = false; else ss << ", ";
      ss << data_ ->parameter_ . network() . name ( d );
    }
    ss << "}";
  }
  a . append ( ss . str () );
  return a;
}

INLINE_IF_HEADER_ONLY std::ostream& operator << ( std::ostream& stream, DomainGraph const& dg ) {
  stream << dg . digraph ();
  return stream;
}


#endif
