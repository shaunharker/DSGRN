/// PatternGraph.hpp
/// MIT LICENSE
/// Shaun Harker and Bree Cummins
/// 2016-03-20

#pragma once

#ifndef INLINE_IF_HEADER_ONLY
#define INLINE_IF_HEADER_ONLY
#endif

#include "Pattern/PatternGraph.h"
#include "Pattern/MatchingRelation.h"

INLINE_IF_HEADER_ONLY PatternGraph::
PatternGraph ( void ) {
  data_ . reset ( new PatternGraph_ );
}

INLINE_IF_HEADER_ONLY PatternGraph::
PatternGraph ( Pattern const& pattern ) {
  assign ( pattern );
}

INLINE_IF_HEADER_ONLY void PatternGraph::
assign ( Pattern const& pattern ) {
  // std::cout << "DEBUG PatternGraph::assign " << __LINE__ << "\n";
  data_ . reset ( new PatternGraph_ );
  data_ -> dimension_ = pattern . dimension ();
  data_ -> size_ = 0;
  // Initialize data structures
  Poset const& poset = pattern . poset ();
  typedef std::set<uint64_t> Clique;
  std::unordered_map<Clique, uint64_t, dsgrn::hash<Clique>> vertices;
  // Add the leaf vertex to the pattern graph, which corresponds to
  // the set of maximal elements in the pattern poset.
  data_ -> leaf_ = data_ -> size_ ++;
  data_ -> labels_ . push_back ( pattern . label () );
  data_ -> consume_ . push_back ( std::unordered_map<uint64_t, uint64_t> () );

  std::set<uint64_t> s;
  for (uint64_t i = 0; i < poset.size(); ++i) s.insert(s.end(), i); // insert-with-hint
  Clique maximal = poset . maximal ( s );

  vertices [ maximal ] = leaf ();
  // Begin bottom-up traversal of pattern graph
  std::stack<Clique> recursion_stack;
  recursion_stack . push ( maximal );
  while ( not recursion_stack . empty () ) {
    //auto print_clique = [](Clique const& c){ std::cout << "["; for ( auto x : c ) std::cout << x << " "; std::cout << "]\n";};
    auto clique = recursion_stack . top ();
    //std::cout << "Inspecting target clique: \n";
    //print_clique(clique);
    uint64_t target = vertices[clique];
    //std::cout << "Target vertex number is " << target << "\n";
    recursion_stack . pop ();
    for ( uint64_t v : clique ) {
      // Determine parent clique
      auto parent_clique = clique;
      parent_clique . erase ( v );
      auto const& parents = poset . parents ( v );
      parent_clique . insert ( parents.begin(), parents.end() );
      parent_clique = poset . maximal ( parent_clique );
      //std::cout << "Inspecting source clique: \n";
      //print_clique(parent_clique);
      // If newly discovered, add it to pattern graph
      if ( vertices . count ( parent_clique ) == 0 ) {
        //std::cout << "Newly discovered.\n";
        vertices[parent_clique] = data_ -> size_ ++;
        //std::cout << "  Assigning index " << vertices[parent_clique] << "\n";
        recursion_stack.push(parent_clique);
        data_ -> labels_ . push_back ( 0 );
        data_ -> consume_ . push_back ( std::unordered_map<uint64_t, uint64_t> () );
      }
      // Add the edge to the pattern graph
      uint64_t source = vertices[parent_clique];
      //std::cout << "Adding edge from " << source << " to " << target << "\n";
      // Use event variable to set consume_ and labels_ data
      auto const& variable = pattern . event ( v );
      //std::cout << "The event variable is " << variable << "\n";
      // Bit fiddling to toggle increasing/decreasing label for "variable"
      uint64_t bit = 1 << variable;
      bit |= bit << pattern . dimension ();
      data_ -> labels_ [ source ] = label(target) ^ bit;
      data_ -> consume_ [ source ] [ label(target) & bit ] = target; 
      // example: consider bits (i+D,i). 01 means Max, 01 means decreasing. Target is decreasing after a Max
    }
  }
  data_ -> root_ = vertices [ std::set<uint64_t> () ];
}

INLINE_IF_HEADER_ONLY uint64_t PatternGraph::
root ( void ) const {
  return data_ -> root_;
}

INLINE_IF_HEADER_ONLY uint64_t PatternGraph::
leaf ( void ) const {
  return data_ -> leaf_;
}

INLINE_IF_HEADER_ONLY uint64_t PatternGraph::
size ( void ) const {
  return data_ -> size_;
}

INLINE_IF_HEADER_ONLY uint64_t PatternGraph::
dimension ( void ) const {
  return data_ -> dimension_;
}

INLINE_IF_HEADER_ONLY uint64_t PatternGraph::
label ( uint64_t v ) const {
  return data_ -> labels_ [ v ];
}

INLINE_IF_HEADER_ONLY uint64_t PatternGraph::
consume ( uint64_t vertex, uint64_t edge_label ) const {
  auto it = data_ -> consume_ [ vertex ] . find ( edge_label );
  if ( it != data_ -> consume_ [ vertex ] . end () ) return it -> second;
  return -1;
}

INLINE_IF_HEADER_ONLY std::string PatternGraph::
graphviz ( void ) const {
  return graphviz_with_highlighted_path ( std::vector<uint64_t> () );
}

INLINE_IF_HEADER_ONLY std::string PatternGraph::
graphviz_with_highlighted_path ( std::vector<uint64_t> const& path ) const {
  std::unordered_set<uint64_t> vertices ( path.begin(), path.end() );
  std::unordered_set<std::pair<uint64_t,uint64_t>, dsgrn::hash<std::pair<uint64_t,uint64_t>>> edges;
  for ( int64_t i = 0; i < (int64_t)path.size() - 1; ++ i ) edges.insert({path[i], path[i+1]});
  MatchingRelation mr(dimension());
  std::stringstream ss;
  ss << "digraph {\n";
  for ( uint64_t vertex = 0; vertex < size (); ++ vertex ) {
    ss << vertex << "[label=\"" << vertex << ":" << mr.vertex_labelstring(label ( vertex ));
    if ( vertex == root() ) ss << "(root)";
    if ( vertex == leaf() ) ss << "(leaf)";
    ss << "\"";
    if ( vertices.count(vertex) ) ss << " color=red";
    ss << "];\n";
  }
  for ( uint64_t source = 0; source < size (); ++ source ) {
    for ( uint64_t variable = 0; variable < 2 * dimension (); ++ variable ) {
      uint64_t edge_label = 1L << variable;
      uint64_t target = consume ( source, edge_label );
      if ( target != -1 ) { 
        ss << source << " -> " << target << " [label=\"" << mr.edge_labelstring(edge_label) << "\"";
        if ( edges.count({source,target}) ) ss << " color = red";
        ss << "];\n";
      }
    }
    ss << source << " -> " << source << " [label=\"" << mr.edge_labelstring(0L) << "\"";
    if ( edges.count({source,source}) ) ss << " color = red";
    ss << "];\n";
  }
  ss << "}\n";
  return ss . str ();
}
