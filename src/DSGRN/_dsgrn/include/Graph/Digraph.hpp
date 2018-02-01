/// Digraph.hpp
/// Shaun Harker
/// 2015-05-24

#pragma once

#ifndef INLINE_IF_HEADER_ONLY
#define INLINE_IF_HEADER_ONLY
#endif

#include "Digraph.h"

INLINE_IF_HEADER_ONLY Digraph::
Digraph ( void ) {
  data_ . reset ( new Digraph_ );
}

INLINE_IF_HEADER_ONLY Digraph::
Digraph ( std::vector<std::vector<uint64_t>> const& adjacencies ) {
  assign ( adjacencies );
}

INLINE_IF_HEADER_ONLY void Digraph::
assign ( std::vector<std::vector<uint64_t>> const& adjacencies ) {
  data_ . reset ( new Digraph_ );
  data_ -> adjacencies_ = adjacencies;
  finalize ();
}

INLINE_IF_HEADER_ONLY std::vector<uint64_t> const& Digraph::
adjacencies ( uint64_t v ) const {
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

INLINE_IF_HEADER_ONLY void Digraph::
finalize ( void ) {
  for ( auto & adj_list : data_ -> adjacencies_ ) sort ( adj_list.begin(), adj_list.end() );
}

INLINE_IF_HEADER_ONLY Digraph Digraph::
transpose ( void ) const {
  uint64_t N = size ();
  std::vector<std::vector<uint64_t>> adjacencies(N);
  for ( uint64_t i = 0; i < N; ++ i ) {
    for ( uint64_t j : data_ -> adjacencies_ [ i ] ) {
      adjacencies[j].push_back(i);
    }
  }
  return Digraph ( adjacencies );
}

INLINE_IF_HEADER_ONLY Digraph Digraph::
transitive_reduction ( void ) const {
  // Algorithm: Remove self-edges, and remove edges which
  //            can be given by a length two path
  // (Assumes the vertices are topologically sorted.)
  // (Assumes original state is transitively closed.)
  uint64_t N = size ();
  Digraph result ( data_ -> adjacencies_ );
  std::vector<std::vector<uint64_t>> & result_adjacencies = 
    result . data_ -> adjacencies_;
  for ( uint64_t u = 0; u < N; ++ u ) {
    std::unordered_set<uint64_t> double_hop;
    double_hop . insert ( u );
    for ( uint64_t v : result_adjacencies [ u ] ) {
      if ( u == v ) continue;
      for ( uint64_t w : result_adjacencies [ v ] ) {
        if ( v == w ) continue;
        double_hop . insert ( w );
      }
    }
    auto removal_predicate = [&] (uint64_t v) {
      return double_hop . count ( v ) ? true : false;
    };
    auto it = std::remove_if ( result_adjacencies[u].begin(),
                               result_adjacencies[u].end(),
                               removal_predicate );
    result_adjacencies[u].erase ( it, result_adjacencies[u].end() );
  }
  return result;
}

INLINE_IF_HEADER_ONLY Digraph Digraph::
transitive_closure ( void ) const {
  uint64_t N = size ();
  // Store adjacency structures in vector<set> format
  std::vector<std::unordered_set<uint64_t>> closure_adjacencies (N); 
  std::vector<std::unordered_set<uint64_t>> transpose_adjacencies (N);  
  for ( uint64_t i = 0; i < N; ++ i ) {
    for ( uint64_t j : data_ -> adjacencies_ [ i ] ) {
      closure_adjacencies[i].insert(j);
      transpose_adjacencies[j].insert(i);
    }
  }
  // Perform transitive closure
  for ( uint64_t k = 0; k < N; ++ k ) {
    for ( uint64_t j : closure_adjacencies[k] ) { 
      for ( uint64_t i : transpose_adjacencies[k] ) {
        if ( i == j ) continue;
        closure_adjacencies[i].insert(j);
        transpose_adjacencies[j].insert(i);
      }
    }
  }
  // Write adjacency lists
  std::vector<std::vector<uint64_t>> result_adjacencies;
  for ( uint64_t i = 0; i < N; ++ i ) {
    result_adjacencies . push_back ( 
      std::vector<uint64_t> ( closure_adjacencies[i] . begin (), 
                              closure_adjacencies[i] . end () ) );
  }
  // Return result
  return Digraph ( result_adjacencies );
}

INLINE_IF_HEADER_ONLY Digraph Digraph::
permute ( const std::vector<uint64_t> & permutation ) const {
  std::vector<std::vector<uint64_t>> new_adjacencies;
  uint64_t N = size();
  new_adjacencies . resize ( N );
  for ( uint64_t i = 0; i < N; ++ i ) {
    uint64_t u = permutation [ i ];
    for ( auto j : data_ -> adjacencies_ [ i ] ) {
      uint64_t v = permutation [ j ];
      new_adjacencies [ u ] . push_back ( v );
    }
  }
  return Digraph ( new_adjacencies );
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
  finalize ();
}


INLINE_IF_HEADER_ONLY std::string Digraph::
graphviz ( void ) const {
  std::stringstream stream;
  stream << "digraph {\n";
  for ( uint64_t v = 0; v < size (); ++ v ) {
    stream << v << ";\n";
  }
  for ( uint64_t source = 0; source < size (); ++ source ) {
    for ( uint64_t target : adjacencies ( source ) ) {
      stream << source << " -> " << target << ";\n";
    }
  }
  stream << "}\n";
  return stream . str ();
}

INLINE_IF_HEADER_ONLY std::ostream& operator << ( std::ostream& stream, Digraph const& dg ) {
  return stream << dg . graphviz ();
}
