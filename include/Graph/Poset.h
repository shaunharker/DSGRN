/// Poset.h
/// Shaun Harker
/// 2015-05-15

#ifndef DSGRN_POSET_H
#define DSGRN_POSET_H

#include "common.h"

#include "Graph/Digraph.h"

struct Poset_;

/// class Poset
class Poset : public Digraph {
public:

  Poset ( void );

  Poset ( std::vector<std::vector<uint64_t>> & adjacencies );

  /// reduction
  ///   Perform a transitive reduction
  ///   (i.e. use Hasse diagram representation)
  void
  reduction ( void );

  /// Return the number of ancestors of node i
  uint64_t
  numberOfAncestors( const uint64_t & i ) const;
  /// Return the number of descendants of node i
  uint64_t
  numberOfDescendants( const uint64_t & i ) const;
  /// Return the number of parents of node i
  uint64_t
  numberOfParents ( const uint64_t & i ) const;
  /// Return the number of children of node i
  uint64_t
  numberOfChildren ( const uint64_t & i ) const;

  /// Check if we have the reachability n -> m
  /// Assume we have a topologically sorted digraph ( n < m )
  bool reachable ( const uint64_t & n, const uint64_t & m ) const;

  /// Reorder the poset according to the vector ordering
  /// ordering[2] = 7 means the node numbered 2 should be numbered now 2
  Poset
  reorder ( const std::vector<uint64_t> & ordering ) const;

private:
  std::shared_ptr<Poset_> dataPoset_;
  //
  void
  computeNumberOfParents ( void) const;
  void
  computeNumberOfChildren ( void ) const;
  void
  computeNumberOfAncestors ( void ) const;
  void
  computeNumberOfDescendants ( void ) const;

  /// serialize
  ///   For use with BOOST Serialization library,
  ///   which is used by the cluster-delegator MPI package
  friend class boost::serialization::access;
  template<class Archive>
  void serialize(Archive & ar, const unsigned int version) {
    ar & boost::serialization::base_object<Digraph>(*this);
    ar & dataPoset_;
  }
};

struct Poset_ {

  std::vector<std::vector<uint64_t>> reachability_;

  std::vector<uint64_t> parentsCount_, childrenCount_;
  std::vector<uint64_t> ancestorsCount_, descendantsCount_;

  /// serialize
  ///   For use with BOOST Serialization library,
  ///   which is used by the cluster-delegator MPI package
  friend class boost::serialization::access;
  template<class Archive>
  void serialize(Archive & ar, const unsigned int version) {
    ar & reachability_;
    ar & parentsCount_;
    ar & childrenCount_;
    ar & ancestorsCount_;
    ar & descendantsCount_;
  }
};

#endif
