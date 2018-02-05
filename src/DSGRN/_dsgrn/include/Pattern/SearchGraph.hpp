/// SearchGraph.hpp
/// MIT LICENSE
/// Shaun Harker and Bree Cummins
/// 2016-03-20

#pragma once

#ifndef INLINE_IF_HEADER_ONLY
#define INLINE_IF_HEADER_ONLY
#endif

#include "SearchGraph.h"
#include "MatchingRelation.h"

SearchGraph::
SearchGraph ( void ) {
  data_ . reset ( new SearchGraph_ );
}

SearchGraph::
SearchGraph ( DomainGraph dg ) {
  assign ( dg );
}

SearchGraph::
SearchGraph ( DomainGraph dg, uint64_t morse_set_index ) {
  assign ( dg, morse_set_index );
}

SearchGraph::
SearchGraph ( std::vector<uint64_t> const& labels, uint64_t dim ) {
  assign ( labels, dim );
}

// TODO: code coverage
void SearchGraph::
assign ( DomainGraph dg ) {
  data_ . reset ( new SearchGraph_ );
  data_ -> dimension_ = dg . dimension ();
  Digraph & digraph = data_ -> digraph_;
  std::unordered_map<uint64_t, uint64_t> domain_to_vertex;
  uint64_t N = dg . digraph() . size ();
  for ( uint64_t domain = 0; domain < N; ++ domain ) {
    data_ -> labels_ . push_back ( dg . label ( domain ) );
  }
  digraph . resize ( N );
  data_ -> event_ . resize ( N );
  for ( uint64_t source = 0; source < N; ++ source ) {
    for ( uint64_t target : dg . digraph() . adjacencies ( source ) ) {
      if ( source == target ) continue; // Don't add self-edge to search graph.
      digraph . add_edge ( source, target );
      data_ -> event_ [ source ] [ target ] = dg . label ( source, target );
    }
  }
  digraph . finalize ();
}

void SearchGraph::
assign ( DomainGraph dg, uint64_t morse_set_index ) {
  data_ . reset ( new SearchGraph_ );
  data_ -> dimension_ = dg . dimension ();
  MorseDecomposition md ( dg . digraph () );
  auto const& morse_set = md . recurrent () [ morse_set_index ];
  Digraph & digraph = data_ -> digraph_;
  std::unordered_map<uint64_t, uint64_t> domain_to_vertex;
  uint64_t N = 0;
  for ( uint64_t domain : morse_set ) {
    domain_to_vertex [ domain ] = N ++;
    data_ -> labels_ . push_back ( dg . label ( domain ) );
    //std::cout << "Domain " << domain << " is vertex " << domain_to_vertex [ domain ] << "\n";
  }
  digraph . resize ( N );
  data_ -> event_ . resize ( N );
  std::vector<uint64_t> domains ( N );
  for ( uint64_t source : morse_set ) {
    uint64_t u = domain_to_vertex[source];
    domains [ u ] = source;
    for ( uint64_t target : dg . digraph() . adjacencies ( source ) ) {
      if ( domain_to_vertex . count ( target ) ) {
        uint64_t v = domain_to_vertex[target];
        digraph . add_edge ( u, v );
        data_ -> event_ [ u ] [ v ] = dg . label ( source, target );
      }
    }
  }
  digraph . finalize ();
  // Debug information (Note: will not survive a serialization/deserialization)
  MatchingRelation mr(dimension());
  data_ -> vertex_information_ = [=](uint64_t v ){
    auto vec_to_string = [](std::vector<uint64_t> const& vec ) {
      std::stringstream ss;
      ss << "(";
      bool first = true;
      for ( auto const& v : vec ) { 
        if ( first ) first = false; else ss << ", ";
        ss << .5 + (float) v;
      }
      ss << ")";
      return ss . str ();
    };
    return vec_to_string(dg.coordinates(domains[v]));
  };
  data_ -> edge_information_ = [=](uint64_t u, uint64_t v ){
    std::stringstream ss;
    uint64_t source = domains[u];
    uint64_t target = domains[v];
    ss << "Transition from " << u << " to " << v << " in searchgraph.\n";
    ss << u << " is domain " << source << " with coordinates " << vertexInformation(u) << "\n";
    ss << "  and label " << mr.vertex_labelstring(dg.label(source)) << "\n";
    ss << v << " is domain " << target << " with coordinates " << vertexInformation(v) << "\n";
    ss << "  and label " << mr.vertex_labelstring(dg.label(target)) << "\n";
    ss << "The direction variable is " << dg . direction ( source, target ) << "\n";
    ss << "The regulator variable is " << dg . regulator ( source, target ) << "\n";
    ss << "(" << u << ", " << v << ") has label " << mr.edge_labelstring(event(u,v)) << "(" << event(u,v) << ")\n";
    return ss.str();
  };
}

// TODO PROBLEM: This code is uncovered by tests.
void SearchGraph::
assign ( std::vector<uint64_t> const& labels, uint64_t dim ) {
  data_ . reset ( new SearchGraph_ );
  data_ -> dimension_ = dim;
  data_ -> labels_ = labels;
  uint64_t N = labels . size ();
  data_ -> digraph_ . resize ( N );
  data_ -> event_ . resize ( N );
  for ( uint64_t v = 0; v < N-1; ++ v ) {
    data_ -> digraph_ . add_edge ( v, v+1 );
    data_ -> event_ [ v ] [ v+1 ] = (label(v) ^ label(v+1)) & label(v+1);
  }
  data_ -> digraph_ . finalize ();
  // Debug information (Note: will not survive a serialization/deserialization)
  MatchingRelation mr(dimension());
  data_ -> vertex_information_ = [=]( uint64_t v ){
    std::stringstream ss;
    ss << "[Vertex " << v << "; Label " << label(v) << "]";
    return ss . str ();
  };
  data_ -> edge_information_ = [=](uint64_t u, uint64_t v ){ //dry
    std::stringstream ss;
    ss << "Transition from " << u << " to " << v << " in searchgraph.\n";
    ss << u << " has label " << mr.vertex_labelstring(label(u)) << "(" << label(u) << ")\n";
    ss << v << " has label " << mr.vertex_labelstring(label(v)) << "(" << label(v) << ")\n";
    ss << "(" << u << ", " << v << ") has label " << mr.edge_labelstring(event(u,v)) << "(" << event(u,v) << ")\n";
    return ss.str();
  };
} 

uint64_t SearchGraph::
size ( void ) const {
  return data_ -> digraph_ . size ();
}

uint64_t SearchGraph::
dimension ( void ) const {
  return data_ -> dimension_;
}

uint64_t SearchGraph::
label ( uint64_t v ) const {
  return data_ -> labels_ [ v ];
}

std::vector<uint64_t> const& SearchGraph::
adjacencies ( uint64_t v ) const {
  return data_ -> digraph_ . adjacencies ( v );
}

uint64_t SearchGraph::
event ( uint64_t source, uint64_t target ) const {
  return data_ -> event_ [ source ] [ target ];
}

std::string SearchGraph::
graphviz ( void ) const {
  return graphviz_with_highlighted_path( {} );
}

INLINE_IF_HEADER_ONLY std::string SearchGraph::
graphviz_with_highlighted_path ( std::vector<uint64_t> const& path ) const {
  std::unordered_set<uint64_t> vertices ( path.begin(), path.end() );
  std::unordered_set<std::pair<uint64_t,uint64_t>, dsgrn::hash<std::pair<uint64_t,uint64_t>>> edges;
  for ( int64_t i = 0; i < (int64_t)path.size() - 1; ++ i ) edges.insert({path[i], path[i+1]});
  MatchingRelation mr(dimension());
  std::stringstream ss;
  ss << "digraph {\n";
  for ( uint64_t vertex = 0; vertex < size (); ++ vertex ) {
    ss << vertex << "[label=\"" << vertex << ":" << mr.vertex_labelstring(label(vertex)) << "\"";
    if ( vertices.count(vertex) ) ss << " color=red";
    ss << "];\n";
  }
  for ( uint64_t source = 0; source < size (); ++ source ) {
    for ( uint64_t target : adjacencies(source) ) {
      ss << source << " -> " << target << " [label=\"" << mr.edge_labelstring(event(source,target)) << "\"";
      if ( edges.count({source,target} ) ) ss << " color=red";
      ss << "];\n";
    }
  }
  ss << "}\n";
  return ss . str ();
}

std::string SearchGraph::
vertexInformation ( uint64_t v ) const {
 return data_ -> vertex_information_ ( v );
}

std::string SearchGraph::
edgeInformation ( uint64_t source, uint64_t target ) const {
  return data_ -> edge_information_ ( source, target );
}
