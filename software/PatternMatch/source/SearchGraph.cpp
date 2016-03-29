/// SearchGraph.h
/// Shaun Harker and Bree Cummins
/// 2016-03-20

#include "SearchGraph.h"

SearchGraph::
SearchGraph ( void ) {
  data_ . reset ( new SearchGraph_ );
}

SearchGraph::
SearchGraph ( DomainGraph const& dg, uint64_t morse_set_index ) {
  assign ( dg, morse_set_index );
}

void SearchGraph::
assign ( DomainGraph const& dg, uint64_t morse_set_index ) {
  data_ . reset ( new SearchGraph_ );
  data_ -> domaingraph_ = dg;
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
  data_ -> domain_ . resize ( N );
  for ( uint64_t source : morse_set ) {
    uint64_t u = domain_to_vertex[source];
    data_ -> domain_ [ u ] = source;
    for ( uint64_t target : dg . digraph() . adjacencies ( source ) ) {
      if ( domain_to_vertex . count ( target ) ) {
        uint64_t v = domain_to_vertex[target];
        digraph . add_edge ( u, v );
        //std::cout << " domain " << source << " leads to domain " << target << "\n";
        //std::cout << " making an edge from " << u << " to " << v << "\n";
        uint64_t direction = dg . direction ( source, target );
        uint64_t regulator = dg . regulator ( source, target );
        //std::cout << " this edge is in the " << direction << " direction\n";
        data_ -> event_ [ u ] [ v ] = _label_event ( label(u), label(v), direction, regulator, data_ -> dimension_ );
        //std::cout << " the edge can support an event on variable " << data_ -> event_ [ u ] [ v ] << "\n";
      }
    }
  }
  digraph . finalize ();
}

DomainGraph const& SearchGraph::
domaingraph ( void ) const {
  return data_ -> domaingraph_;
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
domain ( uint64_t v ) const {
  return data_ -> domain_ [ v ];
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
  auto labelstring = [&](uint64_t L) {
    std::string result;
    for ( uint64_t d = 0; d < dimension(); ++ d ){
      if ( L & ( 1 << d ) ) { 
        result.push_back('D');
      } else if ( L & ( 1 << (d + dimension() ) ) ) { 
        result.push_back('I');
      } else {
        result.push_back('?');
      }
    }
    return result;
  };
  std::stringstream ss;
  ss << "digraph {\n";
  for ( uint64_t vertex = 0; vertex < size (); ++ vertex ) {
    ss << vertex << "[label=\"" << vertex << ":" << labelstring(label(vertex)) << "\"];\n";
  }
  for ( uint64_t source = 0; source < size (); ++ source ) {
    for ( uint64_t target : adjacencies(source) ) {
      ss << source << " -> " << target << " [label=\"" << (int64_t) event(source,target) << "\"];\n";
    }
  }
  ss << "}\n";
  return ss . str ();
}

uint64_t SearchGraph::
_label_event ( uint64_t source_label, uint64_t target_label, 
              uint64_t direction, uint64_t regulator, uint64_t dimension ) const {
  if ( direction == regulator ) return -1;
  uint64_t mask = (1 << regulator) | ( 1 << (regulator + dimension) );
  uint64_t x = source_label & mask;
  uint64_t y = target_label & mask;
  return ( (x != y) || ( x == 0 ) ) ? regulator : -1; 
}

