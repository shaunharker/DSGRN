/// PatternGraph.cpp
/// Shaun Harker and Bree Cummins
/// 2016-03-20

PatternGraph::
PatternGraph ( void ) {
  data_ . reset ( new PatternGraph );
}

PatternGraph::
PatternGraph ( Poset const& poset ) {
  assign ( poset );
}

void PatternGraph::
assign ( Poset const& poset ) {
  data_ . reset ( new PatternGraph );
  // TODO
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

std::vector<uint8_t> PatternGraph::
label ( uint64_t v ) const {
  return data_ -> labels_ [ v ];
}

uint64_t PatternGraph::
consume ( uint64_t vertex, uint64_t variable ) const {
  return data_ -> consume_ [ vertex ] [ variable ];
}
