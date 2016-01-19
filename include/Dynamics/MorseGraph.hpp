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


INLINE_IF_HEADER_ONLY std::ostream& operator << ( std::ostream& stream, MorseGraph const& md ) {
  Poset const poset = md . poset ();
  stream << "digraph {\n";
  for ( uint64_t v = 0; v < poset . size (); ++ v ) {
    stream << v;
    stream << "[label=\"";
    bool first_item = true;
    for ( auto s : md . annotation ( v ) ) {
      if ( first_item ) first_item = false; else stream << ", ";
      stream << s;
    }
    stream << "\"];\n";
  }
  for ( uint64_t source = 0; source < poset . size (); ++ source ) {
    for ( uint64_t target : poset . adjacencies ( source ) ) {
      stream << source << " -> " << target << ";\n";
    }
  }
  stream << "}\n";
  return stream;
}

INLINE_IF_HEADER_ONLY std::string MorseGraph::
SHA256 ( void ) const {
  std::stringstream ss;
  ss << *this;
  return sha256 ( ss . str () );
}

INLINE_IF_HEADER_ONLY void MorseGraph::
_canonicalize ( void ) {

  /// create the original poset numbering : 0 ... N-1
  std::vector<uint64_t> posetOrder;
  for ( uint64_t i=0; i<data_ -> poset_ .size(); ++i ) {
    posetOrder . push_back ( i );
  }

  ///DEBUG
  // std::cout << "Before\n";
  // for ( auto u : posetOrder ) {
  //   std::cout << u << " ";
  // }
  // std::cout << "\n";
  // std::cout << data_ -> poset_;
  /// END DEBUG

  /// Create the sort function
  auto compare = [this](const int & i, const int & j) {

    /// 0) if there is an edge i -> j, we ensure i < j
    if ( data_ -> poset_ . reachable( i, j ) ) {
      return true;
    }
    /// 1) try to sort according to parents
    if ( data_ -> poset_ . numberOfParents(i) < data_ -> poset_ . numberOfParents(j) ) {
      return true;
    }
    if ( data_ -> poset_ . numberOfParents(i) > data_ -> poset_ . numberOfParents(j) ) {
      return false;
    }
    /// 2) try to sort according to ancestors
    if ( data_ -> poset_ . numberOfAncestors(i) < data_ -> poset_ . numberOfAncestors(j) ) {
      return true;
    }
    if ( data_ -> poset_ . numberOfAncestors(i) > data_ -> poset_ . numberOfAncestors(j) ) {
      return false;
    }
    /// 3) Try to sort according to descendants
    if ( data_ -> poset_ . numberOfDescendants(i) < data_ -> poset_ . numberOfDescendants(j) ) {
      return true;
    }
    if ( data_ -> poset_ . numberOfDescendants(i) > data_ -> poset_ . numberOfDescendants(j) ) {
      return false;
    }
    /// 4) Try to sort according to children
    if ( data_ -> poset_ . numberOfChildren(i) < data_ -> poset_ . numberOfChildren(j) ) {
      return true;
    }
    if ( data_ -> poset_ . numberOfChildren(i) > data_ -> poset_ . numberOfChildren(j) ) {
      return false;
    }
    /// 5) Sort according to annotations
    if ( data_ -> annotations_ . find ( i ) -> second . size ( ) <
         data_ -> annotations_ . find ( j ) -> second . size ( ) ) {
      return true;
    }
    if ( data_ -> annotations_ . find ( i ) -> second . size ( ) >
         data_ -> annotations_ . find ( j ) -> second . size ( ) ) {
      return false;
    }
    if ( data_ -> annotations_ . find ( i ) -> second . size ( ) ==
         data_ -> annotations_ . find ( j ) -> second . size ( ) ) {
      uint64_t annotationSize = data_ -> annotations_ . find ( i ) -> second . size ( );
      for ( uint64_t k = 0; k<annotationSize; ++k ) {
        if ( data_ -> annotations_ . find ( i ) -> second[k] <
        data_ -> annotations_ . find ( j ) -> second[k] ) { return true; }
      }
    }
    /// if in case we cannot separate them, use the original node number
    return i < j ;
  };

  /// posetOrder[i] represent the original numbering of the node i
  /// after sort, posetOrder[1] = 7 means the node 1 used to be labelled 7
  sort ( posetOrder.begin(), posetOrder.end(), compare );

  /// DEBUG
  // std::cout << "After\n";
  // for ( auto u : posetOrder ) {
  //   std::cout << u << " ";
  // }
  // std::cout << "\n";
  /// END DEBUG

  /// construct the vector ordering to have
  /// ordering[2] = 9 means node 2 should be relabelled 9
  std::vector<uint64_t> ordering;

  uint64_t N = data_ -> poset_ . size();
  ordering . resize( N );
  for ( uint64_t i=0; i<N; ++i ) {
    ordering [ posetOrder[i] ] = i;
  }

  Poset newPoset = data_ -> poset_ . reorder ( ordering );

  /// std::cout << newPoset;

  data_ -> poset_ = newPoset;

  // std::cout << "Old Annotation\n";
  // for ( uint64_t i=0; i<N; ++i ) {
  //   std::cout << data_ -> annotations_ [ i ] << "\n";
  // }
  // std::cout << data_ -> poset_;

  /// update the Annotation
  std::unordered_map<uint64_t, Annotation> newAnnotations;
  for ( uint64_t i=0; i<N; ++i ) {
    newAnnotations [ ordering[i] ] = data_ -> annotations_ [ i ];
  }

  data_ -> annotations_ = newAnnotations;

  // std::cout << "New Annotation\n";
  // for ( uint64_t i=0; i<N; ++i ) {
  //   std::cout << data_ -> annotations_ [ i ] << "\n";
  // }


}

#endif
