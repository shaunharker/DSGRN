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

  struct NodeToSort {
    uint64_t id;
    uint64_t numberOfParents;
    uint64_t numberOfChildren;
    uint64_t numberOfAncestors;
    uint64_t numberOfDescendants;
    Annotation annotations;
  };

  std::vector<NodeToSort> vn;
  for ( uint64_t v = 0; v < data_ -> poset_ . size (); ++ v ) {
    NodeToSort n;
    n.id = v;
    n.numberOfParents = data_ -> poset_ . numberOfParents(v);
    n.numberOfChildren = data_ -> poset_ . numberOfChildren(v);
    //
    // POTENTIAL BUG :
    // In testing, TestPoset, methods numberOfAncestors(), numberOfDescendants()
    // work but here seems to be off by 1
    //
    n.numberOfAncestors = data_ -> poset_ . numberOfAncestors(v) - 1;
    n.numberOfDescendants = data_ -> poset_ . numberOfDescendants(v) - 1;
    //
    n.annotations = data_ -> annotations_ . find ( v ) -> second;
    vn . push_back ( n );
  }

  // DEBUG
  std::cout << "before\n";
  for ( auto v : vn ) {
    std::cout << "id: " << v.id << " " ;
    std::cout << "Parents: " << v.numberOfParents << " ";
    std::cout << "Children: " << v.numberOfChildren << " ";
    std::cout << "Ancestors: " << v.numberOfAncestors << " ";
    std::cout << "Descendants: " << v.numberOfDescendants << " ";
    std::cout << "\n";
  }
  // END DEBUG


  // Create the sort function
  auto compare = [this](const NodeToSort & i, const NodeToSort & j) {

    // Decide the order of comparison
    // Choice :
    //
    // 0) if there is an edge i -> j, we ensure i < j
    if ( data_ -> poset_ . reachable( i.id, j.id ) ) {
      return true;
    }

    // 1) try to sort according to parents
    //    - enforce root node to come first
    if ( i.numberOfParents < j.numberOfParents ) {
      return true;
    }
    if ( i.numberOfParents > j.numberOfParents ) {
      return false;
    }
    // 2) try to sort according to ancestors
    if ( i.numberOfAncestors < j.numberOfAncestors ) {
      return true;
    }
    if ( i.numberOfAncestors > j.numberOfAncestors ) {
      return false;
    }
    // 3) Try to sort according to descendants (promote deeper branches)
    // Warning if conditions switched
    if ( i.numberOfAncestors < j.numberOfAncestors ) {
      return false;
    }
    if ( i.numberOfAncestors > j.numberOfAncestors ) {
      return true;
    }
    // 4) Try to sort according to children (do not promote leaves)
    // Warning if conditions switched
    if ( i.numberOfChildren < j.numberOfChildren ) {
      return false;
    }
    if ( i.numberOfChildren > j.numberOfChildren ) {
      return true;
    }
    // 5) Sort according to annotations
    if ( i.annotations.size() < j.annotations.size() ) {
      return true;
    }
    if ( i.annotations.size() > j.annotations.size() ) {
      return false;
    }
    if ( i.annotations.size() == j.annotations.size() ) {
      for ( uint64_t k = 0; k<i.annotations.size(); ++k ) {
        if ( i.annotations[k] < j.annotations[k] ) { return true; }
      }
    }
    // if in case we cannot separate them, use the original node number
    return i.id < j.id ;
  };

  sort ( vn.begin(), vn.end(), compare );

  // DEBUG
  std::cout << "after\n";
  for ( auto v : vn ) {
    std::cout << "id: " << v.id << " " ;
    std::cout << "Parents: " << v.numberOfParents << " ";
    std::cout << "Children: " << v.numberOfChildren << " ";
    std::cout << "Ancestors: " << v.numberOfAncestors << " ";
    std::cout << "Descendants: " << v.numberOfDescendants << " ";
    std::cout << "\n";
  }
  // END DEBUG

}

#endif
