/// SearchGraph.h
/// Shaun Harker and Bree Cummins
/// 2016-03-20

#include "SearchGraph.h"

SearchGraph::
SearchGraph ( void ) {
  data_ . reset ( new SearchGraph_ );
}

SearchGraph::
SearchGraph ( DomainGraph dg, uint64_t morse_set_index ) {
  assign ( dg, morse_set_index );
}

SearchGraph::
SearchGraph ( std::vector<uint64_t> const& labels, uint64_t dim ) {
  assign ( labels, dim );
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
        uint64_t direction = dg . direction ( source, target );
        uint64_t regulator = dg . regulator ( source, target );
        data_ -> event_ [ u ] [ v ] = _label_event ( label(u), label(v), direction, regulator, data_ -> dimension_ );
      }
    }
  }
  digraph . finalize ();
  // Debug information (Note: will not survive a serialization/deserialization)
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
    auto domainlabel = [&](uint64_t L) {
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
    uint64_t source = domains[u];
    uint64_t target = domains[v];
    ss << "Transition from " << u << " to " << v << " in searchgraph.\n";
    ss << u << " is domain " << source << " with coordinates " << vertexInformation(u) << "\n";
    ss << "  and label " << domainlabel(dg.label(source)) << "\n";
    ss << v << " is domain " << target << " with coordinates " << vertexInformation(v) << "\n";
    ss << "  and label " << domainlabel(dg.label(target)) << "\n";
    ss << "The direction variable is " << dg . direction ( source, target ) << "\n";
    ss << "The regulator variable is " << dg . regulator ( source, target ) << "\n";
    return ss.str();
  };
}

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
    uint64_t xor_label = label(v) ^ label(v+1);
    uint64_t bit = 1;
    uint64_t & event = data_ -> event_ [ v ] [ v+1 ];
    event = -1;
    for ( uint64_t d = 0; d < dimension (); ++ d ) {
      if ( xor_label & bit ) { 
        event = d;
        break;
      }
      bit <<= 1;
    }
    // DEBUG BEGIN
    if ( event == - 1 ) {
      std::cout << "Failed to learn event.\n";
      abort ();
    }
    // DEBUG END
  }
  data_ -> digraph_ . finalize ();
  // Debug information (Note: will not survive a serialization/deserialization)
  data_ -> vertex_information_ = [=]( uint64_t v ){
    std::stringstream ss;
    ss << "[Vertex " << v << "; Label " << label(v) << "]";
    return ss . str ();
  };
  data_ -> edge_information_ = [=](uint64_t u, uint64_t v ){
    auto humanlabel = [&](uint64_t L) {
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
    ss << "Transition from " << u << " to " << v << " in searchgraph.\n";
    ss << u << " has label " << humanlabel(label(u)) << "(" << label(u) << ")\n";
    ss << v << " has label " << humanlabel(label(v)) << "(" << label(v) << ")\n";
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

std::string SearchGraph::
vertexInformation ( uint64_t v ) const {
 return data_ -> vertex_information_ ( v );
}

std::string SearchGraph::
edgeInformation ( uint64_t source, uint64_t target ) const {
  return data_ -> edge_information_ ( source, target );
}

uint64_t SearchGraph::
_label_event ( uint64_t source_label, uint64_t target_label, 
              uint64_t direction, uint64_t regulator, uint64_t dimension ) const {
  if ( direction == dimension || regulator == dimension || direction == regulator ) return -1;
  uint64_t mask = (1 << regulator) | ( 1 << (regulator + dimension) );
  uint64_t x = source_label & mask;
  uint64_t y = target_label & mask;
  return ( (x != y) || ( x == 0 ) ) ? regulator : -1; 
}

