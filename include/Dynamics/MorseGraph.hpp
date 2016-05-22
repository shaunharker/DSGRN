/// MorseGraph.hpp
/// Shaun Harker
/// 2015-05-24

#ifndef DSGRN_MORSEGRAPH_HPP
#define DSGRN_MORSEGRAPH_HPP

#ifndef INLINE_IF_HEADER_ONLY
#define INLINE_IF_HEADER_ONLY
#endif

#include "MorseGraph.h"

INLINE_IF_HEADER_ONLY MorseGraph::
MorseGraph ( void ) {
  data_ . reset ( new MorseGraph_ );
}

INLINE_IF_HEADER_ONLY MorseGraph::
MorseGraph ( Poset const & ps,
             std::unordered_map<uint64_t, Annotation> const & annotations ) {
  data_ . reset ( new MorseGraph_ );
  data_ -> poset_ = ps;
  data_ -> annotations_ = annotations;
  _canonicalize();
}

INLINE_IF_HEADER_ONLY Poset const MorseGraph::
poset ( void ) const {
  return data_ ->poset_;
}

INLINE_IF_HEADER_ONLY Annotation const MorseGraph::
annotation ( uint64_t v ) const {
  return data_ ->annotations_ . find ( v ) -> second;
}

INLINE_IF_HEADER_ONLY std::string MorseGraph::
stringify ( void ) const {
  std::stringstream ss;
  ss << "{\"poset\":";
  ss << data_ -> poset_ . stringify ();
  ss << ",\"annotations\":[";
  uint64_t N = data_ -> poset_ . size ();
  bool first = true;
  for ( uint64_t v = 0; v < N; ++ v ) {
    if ( first ) first = false; else ss << ",";
    ss << annotation ( v ) . stringify ();
  }
  ss << "]}";
  return ss . str ();
}

INLINE_IF_HEADER_ONLY void MorseGraph::
parse ( std::string const& str ) {
  json mg = json::parse(str);
  data_ -> poset_ . parse ( json::stringify ( mg["poset"] )); //TODO: inefficient
  data_ -> annotations_ . clear ();
  json annotation_array = mg["annotations"];
  uint64_t N = annotation_array . size ();
  for ( uint64_t v = 0; v < N; ++ v ) {
    data_ -> annotations_ [ v ] . parse ( json::stringify ( annotation_array[v] )); //TODO: inefficient
  }
}


INLINE_IF_HEADER_ONLY std::string MorseGraph::
graphviz ( void ) const {
  std::stringstream ss;
  ss << *this;
  return ss . str ();
}

INLINE_IF_HEADER_ONLY std::ostream& operator << ( std::ostream& stream, MorseGraph const& mg ) {
  Poset const poset = mg . poset ();
  stream << "digraph {\n";
  for ( uint64_t v = 0; v < poset . size (); ++ v ) {
    stream << v;
    stream << "[label=\"";
    bool first_item = true;
    for ( auto s : mg . annotation ( v ) ) {
      if ( first_item ) first_item = false; else stream << ", ";
      stream << s;
    }
    stream << "\"];\n";
  }
  for ( uint64_t source = 0; source < poset . size (); ++ source ) {
    for ( uint64_t target : poset . children ( source ) ) {
      stream << source << " -> " << target << ";\n";
    }
  }
  stream << "}\n";
  return stream;
}

INLINE_IF_HEADER_ONLY std::string MorseGraph::
SHA256 ( void ) const {
  return sha256 ( graphviz () );
}

INLINE_IF_HEADER_ONLY void MorseGraph::
_canonicalize ( void ) {
  // Comparison function
  //   The comparison is lexicographical on the following 8-tuple:
  //     1) order in poset (continue if incomparable)
  //     2) number of parents (continue if same)
  //     3) number of ancestors (continue if same)
  //     4) number of descendants (continue if same)
  //     5) number of children (continue if same)
  //     6) number of annotations (continue if same)
  //     7) lexicographical sort of annotations (continue if same)
  //     8) vertex index number
  auto compare = [this](const int & i, const int & j) {
    if ( i == j ) return false;
    // Order in poset
    auto & poset = data_ -> poset_;
    if ( poset . compare ( i, j ) ) return true;
    if ( poset . compare ( j, i ) ) return false;
    // Parent count
    uint64_t A, B;
    A = poset . parents(i) . size();
    B = poset . parents(j) . size();
    if ( A != B ) return A < B;
    // Ancestor count
    A = poset . ancestors(i) . size();
    B = poset . ancestors(j) . size();
    if ( A != B ) return A < B;
    // Descendant count
    A = poset . descendants(i) . size();
    B = poset . descendants(j) . size();
    if ( A != B ) return A < B;
    // Children count
    A = poset . children(i) . size();
    B = poset . children(j) . size();
    if ( A != B ) return A < B;
    // Annotation count
    Annotation const& annotations_i = data_ -> annotations_ . find ( i ) -> second;
    Annotation const& annotations_j = data_ -> annotations_ . find ( j ) -> second;
    A = annotations_i. size ();
    B = annotations_j. size ();
    if ( A != B ) return A < B;
    // Annotation lexicographical ordering
    uint64_t num_labels = A;
    for ( uint64_t k = 0; k < num_labels; ++ k ) {
      std::string const& label_i = annotations_i[k];
      std::string const& label_j = annotations_j[k];
      if ( label_i != label_j ) return label_i < label_j;
    }
    // Vertex index ordering
    return i < j;
  };

  uint64_t N = data_ -> poset_ . size ();

  // Construct "permutation" such that permutation[i] holds the 
  // new vertex index we would like to give to vertex i
  std::vector<uint64_t> permutation ( N );
  std::vector<uint64_t> inverse_permutation ( N );
  for ( uint64_t i = 0; i < N; ++ i ) inverse_permutation[i]=i;
  std::sort ( inverse_permutation.begin(), inverse_permutation.end(), compare );
  for ( uint64_t i = 0; i < N; ++ i ) permutation[inverse_permutation[i]] = i;

  // Update the MorseGraph
  data_ -> poset_ = data_ -> poset_ . permute ( permutation );
  std::unordered_map<uint64_t, Annotation> annotations;
  for ( uint64_t i = 0; i < N; ++ i ) {
    annotations[permutation[i]] = data_ -> annotations_ [ i ];
  }
  data_ -> annotations_ = annotations;
}

#endif
