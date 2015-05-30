/// Poset.h
/// Shaun Harker
/// 2015-05-15

#ifndef DSGRN_POSET_H
#define DSGRN_POSET_H

#include "Graph/Digraph.h"

#include <algorithm>
#include <cstdlib>
#include <unordered_set>

#include "boost/serialization/serialization.hpp"
#include "boost/serialization/base_object.hpp"

/// class Poset_
class Poset_ : public Digraph_ {
public:
  /// reduction
  ///   Perform a transitive reduction
  ///   (i.e. use Hasse diagram representation)
  void
  reduction ( void );
private:
  /// serialize
  ///   For use with BOOST Serialization library,
  ///   which is used by the cluster-delegator MPI package
  friend class boost::serialization::access;
  template<class Archive>
  void serialize(Archive & ar, const unsigned int version) {
    ar & boost::serialization::base_object<Digraph_>(*this);
  }
};

inline void Poset_::
reduction ( void ) {
  // Algorithm: Remove self-edges, and remove edges which 
  // can be given by a double-hop
  // (Assumes the vertices are topologically sorted.)
  // (Assumes original state is transitively closed.)
  uint64_t N = size ();
  for ( uint64_t u = 0; u < N; ++ u ) {
    std::unordered_set<uint64_t> double_hop;
    double_hop . insert ( u );
    for ( uint64_t v : adjacencies_ [ u ] ) {
      if ( u == v ) continue;
      for ( uint64_t w : adjacencies_ [ v ] ) {
        if ( v == w ) continue;
        double_hop . insert ( w );
      }
    }
    auto removal_predicate = [&] (uint64_t v) { 
      return double_hop . count ( v ) ? true : false;
    };
    auto it = std::remove_if ( adjacencies_[u].begin(), 
                               adjacencies_[u].end(), 
                               removal_predicate );
    adjacencies_[u].erase ( it, adjacencies_[u].end() );
  } 
}
#endif
