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
  // std::cout << "DEBUG PatternGraph::assign " << __LINE__ << "\n";
  data_ . reset ( new PatternGraph_ );
  data_ -> dimension_ = pattern . dimension ();
  data_ -> size_ = 0;
  // Initialize data structures
  Poset const& poset = pattern . poset ();
  typedef std::set<uint64_t> Clique;
  std::unordered_map<Clique, uint64_t, boost::hash<Clique>> vertices;
  // Add the leaf vertex to the pattern graph, which corresponds to
  // the set of maximal elements in the pattern poset.
  data_ -> leaf_ = data_ -> size_ ++;
  data_ -> labels_ . push_back ( pattern . label () );
  data_ -> consume_ . push_back ( std::unordered_map<uint64_t, uint64_t> () );
  Clique maximal = poset . maximal ( std::set<uint64_t> ( 
    boost::counting_iterator<uint64_t>(0), 
    boost::counting_iterator<uint64_t> ( poset.size() ) ) ); 
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
      data_ -> consume_ [ source ] [ variable ] = target;
      // Bit fiddling to toggle increasing/decreasing label for "variable"
      uint64_t bit = 1 << variable;
      bit |= bit << pattern . dimension ();
      data_ -> labels_ [ source ] = label(target) ^ bit;
    }
  }
  data_ -> root_ = vertices [ std::set<uint64_t> () ];
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
dimension ( void ) const {
  return data_ -> dimension_;
}

uint64_t PatternGraph::
label ( uint64_t v ) const {
  return data_ -> labels_ [ v ];
}

uint64_t PatternGraph::
consume ( uint64_t vertex, uint64_t variable ) const {
  auto it = data_ -> consume_ [ vertex ] . find ( variable );
  if ( it != data_ -> consume_ [ vertex ] . end () ) return it -> second;
  return -1;
}

std::string PatternGraph::
graphviz ( void ) const {
  auto labelstring = [&](uint64_t L) {
    std::string result;
    for ( uint64_t d = 0; d < dimension(); ++ d ){
      if ( L & (1 << d) ) result.push_back('D'); else result.push_back('I');
    }
    return result;
  };
  std::stringstream ss;
  ss << "digraph {\n";
  for ( uint64_t vertex = 0; vertex < size (); ++ vertex ) {
    ss << vertex << "[label=\"" << vertex << ":" << labelstring(label ( vertex ));
    if ( vertex == root() ) ss << "(root)";
    if ( vertex == leaf() ) ss << "(leaf)";
    ss << "\"];\n";
  }
  for ( uint64_t source = 0; source < size (); ++ source ) {
    for ( uint64_t variable = 0; variable < dimension (); ++ variable ) {
      uint64_t target = consume ( source, variable );
      if ( target != -1 ) { 
        ss << source << " -> " << target << " [label=\"" << variable << "\"];\n";
      }
    }
  }
  ss << "}\n";
  return ss . str ();
}
