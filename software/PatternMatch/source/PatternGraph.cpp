/// PatternGraph.cpp
/// Shaun Harker and Bree Cummins
/// 2016-03-20

#include "PatternGraph.h"
#include <boost/functional/hash.hpp>
#include <boost/iterator/counting_iterator.hpp>

PatternGraph::
PatternGraph ( void ) {
  data_ . reset ( new PatternGraph_ );
}

PatternGraph::
PatternGraph ( Pattern const& pattern ) {
  assign ( pattern );
}

void PatternGraph::
assign ( Pattern const& pattern ) {
  data_ . reset ( new PatternGraph_ );
  // Initialize data structures
  Digraph digraph;
  Poset const& poset = pattern . poset ();
  typedef std::set<uint64_t> Clique;
  std::unordered_map<Clique, uint64_t, boost::hash<Clique>> vertices;
  // Add the leaf vertex to the pattern graph, which corresponds to
  // the set of maximal elements in the pattern poset.
  data_ -> leaf_ = digraph . add_vertex ();
  data_ -> labels_ [ leaf () ] = pattern . label ();
  Clique maximal = poset . maximal ( std::set<uint64_t> ( 
    boost::counting_iterator<uint64_t>(0), 
    boost::counting_iterator<uint64_t> ( poset.size() ) ) ); 
  vertices [ maximal ] = leaf ();
  // Begin bottom-up traversal of pattern graph
  std::stack<Clique> recursion_stack;
  recursion_stack . push ( maximal );
  while ( not recursion_stack . empty () ) {
    auto clique = recursion_stack . top ();
    uint64_t target = vertices[clique];
    recursion_stack . pop ();
    for ( uint64_t v : clique ) {
      // Determine parent clique
      auto parent_clique = clique;
      parent_clique . erase ( v );
      auto const& parents = poset . parents ( v );
      parent_clique . insert ( parents.begin(), parents.end() );
      parent_clique = poset . maximal ( parent_clique );
      // If newly discovered, add it to pattern graph
      if ( vertices . count ( parent_clique ) == 0 ) {
        vertices[parent_clique] = digraph . add_vertex ();
        recursion_stack.push(parent_clique);
        data_ -> labels_ . push_back ( 0 );
        data_ -> consume_ . push_back ( std::unordered_map<uint64_t, uint64_t> () );
      }
      // Add the edge to the pattern graph
      uint64_t source = vertices[parent_clique];
      digraph . add_edge ( source, target );
      // Use event variable to set consume_ and labels_ data
      auto const& variable = pattern . event ( v );
      data_ -> consume_ [ source ] [ variable ] = target;
      // Bit fiddling to toggle increasing/decreasing label for "variable"
      uint64_t bit = 1 << variable;
      bit |= bit << pattern . dimension ();
      data_ -> labels_ [ source ] = label(target) ^ bit;
    }
  }
  data_ -> root_ = vertices [ std::set<uint64_t> () ];
  if ( label ( root () ) != label ( leaf () ) ) {
    throw std::logic_error ( "Invalid pattern" );
  }
}

uint64_t PatternGraph::
root ( void ) const {
  return data_ -> root_;
}

uint64_t PatternGraph::
leaf ( void ) const {
  return data_ -> leaf_;
}

uint64_t PatternGraph::
size ( void ) const {
  return data_ -> size_;
}

uint64_t PatternGraph::
label ( uint64_t v ) const {
  return data_ -> labels_ [ v ];
}

uint64_t PatternGraph::
consume ( uint64_t vertex, uint64_t variable ) const {
  return data_ -> consume_ [ vertex ] [ variable ];
}
