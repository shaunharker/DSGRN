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
  data_ -> dimension_ = D;
  std::vector<uint64_t> limits = parameter . network() . domains ();
  std::vector<uint64_t> jump ( D ); // index offset in each dim
  uint64_t N = 1;
  for ( uint64_t d = 0; d < D; ++ d ) {
    jump[d] =  N;
    N *=  limits [ d ];
    data_ -> direction_ [ jump[d] ] = d;
  }
  data_ -> digraph_ = Digraph ();
  data_ -> digraph_ . resize ( N );
  data_ -> labelling_ = parameter . labelling ();
  Digraph & digraph = data_ -> digraph_;
  std::vector<uint64_t> & labelling = data_ -> labelling_;
  for ( uint64_t i = 0; i < N; ++ i ) {
    if ( labelling [ i ] == 0 ) {
      digraph . add_edge ( i, i );
    }
    uint64_t leftbit = 1;
    uint64_t rightbit = (1LL << D);
    for ( int d = 0; d < D; ++ d, leftbit <<= 1, rightbit <<= 1 ) {
      if ( labelling [ i ] & rightbit ) {
        uint64_t j = i + jump[d];
        if ( not (labelling [ j ] & leftbit) ) {
          digraph . add_edge ( i, j );
        }
      }
      if ( labelling [ i ] & leftbit ) {
        uint64_t j = i - jump[d];
        if ( not (labelling [ j ] & rightbit) ) {
          digraph . add_edge ( i, j );
        }
      }
    }
  }
  digraph . finalize ();
}

INLINE_IF_HEADER_ONLY Digraph const DomainGraph::
digraph ( void ) const {
  return data_ -> digraph_;
}

INLINE_IF_HEADER_ONLY uint64_t DomainGraph::
dimension ( void ) const {
  return data_ -> dimension_;
}

INLINE_IF_HEADER_ONLY std::vector<uint64_t> DomainGraph::
coordinates ( uint64_t domain ) const {
  std::vector<uint64_t> result ( dimension () );
  std::vector<uint64_t> limits = data_ -> parameter_ . network() . domains ();
  for ( int d = 0; d < dimension(); ++ d ) { 
    result[d] = domain % limits[d];
    domain = domain / limits[d];
  }
  return result;
}

INLINE_IF_HEADER_ONLY uint64_t DomainGraph::
label ( uint64_t domain ) const {
  return data_ -> labelling_ [ domain ];
}

INLINE_IF_HEADER_ONLY uint64_t DomainGraph::
direction ( uint64_t source, uint64_t target ) const {
  if ( source == target ) return dimension ();
  return data_ -> direction_ [ std::abs((int64_t)source-(int64_t)target) ];
}

INLINE_IF_HEADER_ONLY uint64_t DomainGraph::
regulator ( uint64_t source, uint64_t target ) const {
  if ( source == target ) return dimension ();
  std::vector<uint64_t> limits = data_ -> parameter_ . network() . domains ();
  uint64_t variable = direction ( source, target );
  uint64_t domain = std::min(source,target);
  for ( int d = 0; d < variable; ++ d ) domain = domain / limits[d];
  uint64_t threshold = domain % limits[variable];
  return data_ -> parameter_ . regulator ( variable, threshold );
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
      ss << data_ -> parameter_ . network() . name ( d );
    }
    ss << "}";
  }
  a . append ( ss . str () );
  return a;
}

INLINE_IF_HEADER_ONLY std::string DomainGraph::
graphviz ( void ) const {
  return digraph () . graphviz (); 
}

INLINE_IF_HEADER_ONLY std::ostream& operator << ( std::ostream& stream, DomainGraph const& dg ) {
  stream << dg . digraph ();
  return stream;
}

#endif
