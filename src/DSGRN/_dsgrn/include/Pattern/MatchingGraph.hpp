/// MatchingGraph.cpp
/// MIT LICENSE
/// Shaun Harker and Bree Cummins
/// 2016-03-20

#pragma once

#ifndef INLINE_IF_HEADER_ONLY
#define INLINE_IF_HEADER_ONLY
#endif

#include "MatchingGraph.h"

INLINE_IF_HEADER_ONLY MatchingGraph::
MatchingGraph ( void ) {
  data_ . reset ( new MatchingGraph_ );
}

INLINE_IF_HEADER_ONLY MatchingGraph::
MatchingGraph ( SearchGraph const& sg, PatternGraph const& pg ) {
  assign ( sg, pg );
}

INLINE_IF_HEADER_ONLY void MatchingGraph::
assign ( SearchGraph const& sg, PatternGraph const& pg ) {
  data_ . reset ( new MatchingGraph_ );
  data_ -> sg_ = sg;
  data_ -> pg_ = pg;
}

INLINE_IF_HEADER_ONLY SearchGraph const& MatchingGraph::
searchgraph ( void ) const {
  return data_ ->  sg_;
}

INLINE_IF_HEADER_ONLY PatternGraph const& MatchingGraph::
patterngraph ( void ) const {
  return data_ -> pg_;
}

INLINE_IF_HEADER_ONLY bool MatchingGraph::
query ( Vertex const& v ) const {
  uint64_t search_label = searchgraph().label(v.first);
  uint64_t pattern_label = patterngraph().label(v.second);
  return (pattern_label & search_label) == search_label;
}

INLINE_IF_HEADER_ONLY std::vector<MatchingGraph::Vertex> MatchingGraph::
adjacencies ( Vertex const& v ) const {
  std::vector<Vertex> result;
  uint64_t const& domain = v . first;
  uint64_t const& position = v . second;
  for ( auto nextdomain : searchgraph() . adjacencies(domain) ) {
    // Check for intermediate match
    if ( query ( {nextdomain, position} ) ) result . push_back ( {nextdomain, position} );
    // Check for extremal match
    // Determine which event occurs between domains (min/max on a given variable, encoded as signed integer
    uint64_t edge_label = searchgraph() . event ( domain, nextdomain );
    // Find successor in pattern graph which consumes event
    uint64_t nextposition = patterngraph() . consume ( position, edge_label );
    if ( nextposition == -1 ) continue;
    if ( query ( {nextdomain, nextposition} ) ) result . push_back ( {nextdomain, nextposition} );
  }
  std::sort ( result.begin(), result.end() );
  return result;
}

INLINE_IF_HEADER_ONLY std::vector<MatchingGraph::Vertex> MatchingGraph::
roots ( void ) const {
  std::vector<Vertex> result;
  uint64_t root = patterngraph().root();
  for ( uint64_t domain = 0; domain < searchgraph().size(); ++ domain ) {
    if ( query ( {domain, root} ) ) result . push_back ( {domain, root} );
  }
  return result;
}

INLINE_IF_HEADER_ONLY uint64_t MatchingGraph::
domain ( Vertex const& v ) const {
  return v . first;
}

INLINE_IF_HEADER_ONLY uint64_t MatchingGraph::
position ( Vertex const& v ) const {
  return v . second;
}

INLINE_IF_HEADER_ONLY MatchingGraph::Vertex MatchingGraph::
vertex ( uint64_t domain, uint64_t position ) const {
  return Vertex ( {domain, position} );
}

INLINE_IF_HEADER_ONLY std::string MatchingGraph::
graphviz ( void ) const {
  return graphviz_with_highlighted_path( {} );
}

INLINE_IF_HEADER_ONLY std::string MatchingGraph::
graphviz_with_highlighted_path ( std::vector<MatchingGraph::Vertex> const& path ) const {
  // traverse graph to learn all vertices
  std::set<Vertex> vertices;
  std::stack<Vertex> dfs;
  for ( Vertex const& v : roots () ) dfs . push ( v );
  while ( not dfs . empty () ) {
    Vertex v = dfs . top ();
    dfs . pop ();
    if ( vertices . count ( v ) ) continue;
    vertices . insert ( v );
    for ( Vertex const& u : adjacencies ( v ) ) {
      if ( vertices . count ( u ) ) continue;
      dfs . push(u);
    }
  }

  // lookup for path
  std::unordered_set<Vertex, dsgrn::hash<Vertex>> path_vertices ( path.begin(), path.end() );
  std::unordered_set<std::pair<Vertex,Vertex>, dsgrn::hash<std::pair<Vertex,Vertex>>> path_edges; 
  for ( int64_t i = 0; i < (int64_t) path.size() - 1; ++ i ) path_edges.insert({path[i], path[i+1]});

  // build string
  std::stringstream ss;
  std::unordered_map<Vertex,uint64_t, dsgrn::hash<Vertex>> indexing;
  ss << "digraph {\n";
  // vertices
  for ( Vertex const& v : vertices ) {
    uint64_t index = indexing . size ();
    indexing[v] = index;
    ss << index << "[label=\"(" << domain(v) << "," << position(v) << ")\""; 
    if ( path_vertices.count(v) ) ss << " color=red";
    ss << "];\n";
  }
  // edges
  for ( Vertex const& source : vertices ) {
    for ( Vertex const& target : adjacencies(source) ) {
      ss << indexing[source] << " -> " << indexing[target];
      if ( path_edges.count({source,target}) ) ss << " [color=red]";
      ss << ";\n";
    }
  }
  ss << "}\n";
  return ss . str ();
}
