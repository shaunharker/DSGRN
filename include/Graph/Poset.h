/// Poset.h
/// Shaun Harker
/// 2015-05-15

#ifndef DSGRN_POSET_H
#define DSGRN_POSET_H

#include "common.h"

#include "Graph/Digraph.h"

/// class Poset
class Poset : public Digraph {
public:
  /// reduction
  ///   Perform a transitive reduction
  ///   (i.e. use Hasse diagram representation)
  void
  reduction ( void );

  // Return the number of ancestors of node i
  uint64_t numberOfAncestors( const uint64_t & i ) const;
  // Return the number of descendants of node i
  uint64_t numberOfDescendants( const uint64_t & i ) const;
  // Return the number of parents of node i
  uint64_t numberOfParents ( const uint64_t & i ) const;
  // Return the number of children of node i
  uint64_t numberOfChildren ( const uint64_t & i ) const;

private:
  std::vector<std::vector<uint64_t>> reachability_;
  std::vector<std::vector<uint64_t>> transitiveReduction_;

  void sumRowReachability  ( void );
  std::vector<uint64_t> sumRowReachability_;
  void sumColumnReachability  ( void );
  std::vector<uint64_t> sumColumnReachability_;
  void sumRowTransitiveReduction  ( void );
  std::vector<uint64_t> sumRowTransitiveReduction_;
  void sumColumnTransitiveReduction  ( void );
  std::vector<uint64_t> sumColumnTransitiveReduction_;


  // std::vector<std::vector<uint64_t>> reachabilityMatrix_;
  // std::vector<std::vector<uint64_t>> transitiveReductionMatrix_;
  // void constructReachabilityMatrix ( void );
  // void constructTransitiveReductionMatrix ( void );

  // pre-compute some quantities
  // void sumRowReachability  ( void );
  // std::vector<uint64_t> sumRowReachability_;
  // void sumColumnReachability  ( void );
  // std::vector<uint64_t> sumColumnReachability_;



  /// serialize
  ///   For use with BOOST Serialization library,
  ///   which is used by the cluster-delegator MPI package
  friend class boost::serialization::access;
  template<class Archive>
  void serialize(Archive & ar, const unsigned int version) {
    ar & boost::serialization::base_object<Digraph>(*this);
  }
};

#endif
