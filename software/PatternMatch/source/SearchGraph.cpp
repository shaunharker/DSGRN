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
  data_ -> dimension_ = dg . dimension ();
  MorseDecomposition md ( dg . digraph () );
  auto & morse_set = md . recurrent () [ morse_set_index ];
  Digraph & digraph = data_ -> digraph_;
  std::unordered_map<uint64_t, uint64_t> domain_to_vertex;
  uint64_t N = 0;
  for ( uint64_t domain : morse_set ) {
    domain_to_vertex [ domain ] = N ++;
    data_ -> labels_ . push_back ( dg . label ( domain ) );
  }
  digraph . resize ( N );
  data_ -> switching_ . resize ( N );
  for ( uint64_t source : morse_set ) {
    uint64_t u = domain_to_vertex[source];
    for ( uint64_t target : dg . adjacencies ( domain ) ) {
      if ( domain_to_vertex . count ( target ) ) {
        uint64_t v = domain_to_vertex[target];
        digraph . add_edge ( u, v );
        uint64_t direction = dg . direction ( source, target );
        data_ -> event_ [ u ] [ v ] = _label_event ( label(u), label(v), direction, data_ -> dimension_ );
      }
    }
  }
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


uint64_t SearchGraph::
_label_event ( uint64_t source_label, uint64_t target_label, 
              uint64_t direction, uint64_t dimension ) const {
  // lookup[2^i] == i for 0 <= i < 32, lookup[0] = -1
  static const std::unordered_map<uint64_t,uint64_t> lookup = 
    { {1, 0}, {2, 1}, {4, 2}, {8, 3}, 
      {16, 4}, {32, 5}, {64, 6}, {128, 7}, 
      {256, 8}, {512, 9}, {1024, 10}, {2048, 11}, 
      {4096, 12}, {8192, 13}, {16384, 14}, {32768, 15}, 
      {65536, 16}, {131072, 17}, {262144, 18}, {524288, 19}, 
      {1048576, 20}, {2097152, 21}, {4194304, 22}, {8388608, 23}, 
      {16777216, 24}, {33554432, 25}, {67108864, 26}, {134217728, 27}, 
      {268435456, 28}, {536870912, 29}, {1073741824, 30}, {2147483648, 31},
      {0, -1}
    };
  // Mask first D bits, apart from direction variable
  uint64_t mask = (1 << dimension - 1) ^ (1 << direction);
  // Apply mask to determine 
  uint64_t one_hot = (source_label ^ target_label) & mask;
  return lookup [ one_hot ];
}

