/// Digraph.hpp
/// Shaun Harker
/// 2015-05-24

#ifndef DSGRN_DIGRAPH_HPP
#define DSGRN_DIGRAPH_HPP

#ifndef INLINE_IF_HEADER_ONLY
#define INLINE_IF_HEADER_ONLY
#endif

#include "Digraph.h"

INLINE_IF_HEADER_ONLY Digraph::
Digraph ( void ) {
  data_ . reset ( new Digraph_ );
}

INLINE_IF_HEADER_ONLY Digraph::
Digraph ( std::vector<std::vector<uint64_t>> & adjacencies ) {
  data_ . reset ( new Digraph_ );
  data_ -> adjacencies_ = adjacencies;
}

INLINE_IF_HEADER_ONLY std::vector<uint64_t> const& Digraph::
adjacencies ( uint64_t v ) const {
  return data_ -> adjacencies_ [ v ];
}

INLINE_IF_HEADER_ONLY std::vector<uint64_t> & Digraph::
adjacencies ( uint64_t v ) {
  return data_ -> adjacencies_ [ v ];
}

INLINE_IF_HEADER_ONLY uint64_t Digraph::
size ( void ) const {
  return data_ -> adjacencies_.size();
}

INLINE_IF_HEADER_ONLY void Digraph::
resize ( uint64_t n ) {
  data_ -> adjacencies_ . resize ( n );
}

INLINE_IF_HEADER_ONLY uint64_t Digraph::
add_vertex ( void ) {
  data_ -> adjacencies_ . push_back ( std::vector<uint64_t> () );
  return data_ -> adjacencies_ . size () - 1;
}

INLINE_IF_HEADER_ONLY void Digraph::
add_edge ( uint64_t source, uint64_t target ) {
  data_ -> adjacencies_[source].push_back(target);
}

INLINE_IF_HEADER_ONLY std::string Digraph::
stringify ( void ) const {
  std::stringstream ss;
  ss << "[";
  bool outerfirst = true;
  for ( auto const& adjlist : data_ -> adjacencies_ ) {
    if ( outerfirst ) outerfirst = false; else ss << ",";
    ss << "[";
    bool innerfirst = true;
    for ( uint64_t v : adjlist ) {
      if ( innerfirst ) innerfirst = false; else ss << ",";
      ss << v;
    }
    ss << "]";
  }
  ss << "]";
  return ss . str ();
}

INLINE_IF_HEADER_ONLY void Digraph::
parse ( std::string const& str ) {
  json array = json::parse(str);
  uint64_t N = array . size ();
  data_ -> adjacencies_ . clear ();
  data_ -> adjacencies_ . resize ( N );
  for ( uint64_t source = 0; source < N; ++ source ) {
    json adjlist = array[source];
    for ( auto target : adjlist ) {
      data_ -> adjacencies_[source] . push_back ( target );
    }
  }
}

INLINE_IF_HEADER_ONLY std::ostream& operator << ( std::ostream& stream, Digraph const& dg ) {
  stream << "digraph {\n";
  for ( uint64_t v = 0; v < dg . size (); ++ v ) {
    stream << v << ";\n";
  }
  for ( uint64_t source = 0; source < dg . size (); ++ source ) {
    for ( uint64_t target : dg . adjacencies ( source ) ) {
      stream << source << " -> " << target << ";\n";
    }
  }
  stream << "}\n";
  return stream;
}


INLINE_IF_HEADER_ONLY void Digraph::
sortAdjacencies ( void ) {
  // Sort the adjacency list in increasing order
  for ( auto & u : data_ -> adjacencies_ ) {
    sort ( u.begin(), u.end() );
  }
}


#endif
