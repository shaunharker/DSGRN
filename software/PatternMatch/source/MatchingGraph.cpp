/// MatchingGraph.cpp
/// Shaun Harker and Bree Cummins
/// 2016-03-20

#include "MatchingGraph.h"


MatchingGraph::
MatchingGraph ( void ) {
  data_ . reset ( new MatchingGraph_ );
}

MatchingGraph::
MatchingGraph ( SearchGraph const& sg, PatternGraph const& pg ) {
  assign ( sg, pg );
}

void MatchingGraph::
assign ( SearchGraph const& sg, PatternGraph const& pg ) {
  data_ . reset ( new MatchingGraph_ );
  data_ -> sg_ = sg;
  data_ -> pg_ = pg;
}

SearchGraph const& MatchingGraph::
searchgraph ( void ) const {
  return data_ ->  sg_;
}

PatternGraph const& MatchingGraph::
patterngraph ( void ) const {
  return data_ -> pg_;
}

bool MatchingGraph::
query ( Vertex const& v ) const {
  uint64_t search_label = searchgraph().label(v.first);
  uint64_t pattern_label = patterngraph().label(v.second);
  return (pattern_label & search_label) == search_label;
}

std::vector<MatchingGraph::Vertex> MatchingGraph::
adjacencies ( Vertex const& v ) const {
  std::vector<Vertex> result;
  uint64_t const& domain = v . first;
  uint64_t const& position = v . second;
  for ( auto nextdomain : searchgraph() . adjacencies(domain) ) {
    // Check for intermediate match
    if ( query ( {nextdomain, position} ) ) result . push_back ( {nextdomain, position} );
    // Check for extremal match
    // Determine what variable can have a min/max event 
    uint64_t variable = searchgraph() . event ( domain, nextdomain );
    if ( variable == -1 ) continue;
    // Find successor in pattern graph which consumes event
    uint64_t nextposition = patterngraph() . consume ( position, variable );
    if ( nextposition == -1 ) continue;
    if ( query ( {nextdomain, nextposition} ) ) result . push_back ( {nextdomain, nextposition} );
  }
  std::sort ( result.begin(), result.end() );
  return result;
}

std::vector<MatchingGraph::Vertex> MatchingGraph::
roots ( void ) const {
  std::vector<Vertex> result;
  uint64_t root = patterngraph().root();
  for ( uint64_t domain = 0; domain < searchgraph().size(); ++ domain ) {
    if ( query ( {domain, root} ) ) result . push_back ( {domain, root} );
  }
  return result;
}

uint64_t MatchingGraph::
domain ( Vertex const& v ) const {
  return v . first;
}

uint64_t MatchingGraph::
position ( Vertex const& v ) const {
  return v . second;
}

MatchingGraph::Vertex MatchingGraph::
vertex ( uint64_t domain, uint64_t position ) const {
  return Vertex ( {domain, position} );
}

std::string MatchingGraph::
graphviz ( void ) const {
  std::stringstream ss;
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
  std::unordered_map<Vertex,uint64_t,boost::hash<Vertex>> indexing;
  ss << "digraph {\n";
  for ( Vertex const& v : vertices ) {
    uint64_t index = indexing . size ();
    indexing[v] = index;
    ss << index << "[label=\"(" << domain(v) << "," << position(v) << ")\"];\n";
  }
  for ( Vertex const& source : vertices ) {
    for ( Vertex const& target : adjacencies(source) ) {
      ss << indexing[source] << " -> " << indexing[target] << ";\n";
    }
  }
  ss << "}\n";
  return ss . str ();
}

bool MatchingGraph::
_match ( uint64_t search_label, uint64_t pattern_label ) const {
  // auto labelstring = [&](uint64_t L) {
  //   std::string result;
  //   for ( uint64_t d = 0; d < searchgraph().dimension(); ++ d ){
  //     if ( L & ( 1 << d ) ) {
  //       result.push_back('D');
  //     } else if ( L & ( 1 << (d + searchgraph().dimension() ) ) ) {
  //       result.push_back('I');
  //     } else {
  //       result.push_back('?');
  //     }
  //   }
  //   return result;
  // };
  // std::cout << "search_label = " << labelstring(search_label) << " pattern_label = " << labelstring(pattern_label) << "\n";
  // std::cout << "search_label = " << (search_label) << " pattern_label = " << (pattern_label) << "\n";
  // std::cout << (pattern_label & search_label) << "\n";
  // std::cout << (((pattern_label & search_label) == search_label) ? "match\n" : "no match\n");
  return (pattern_label & search_label) == search_label;
}
