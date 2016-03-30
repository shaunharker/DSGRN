/// OrderParameter.h
/// Shaun Harker
/// 2015-05-24

#ifndef DSGRN_ORDERPARAMETER_H
#define DSGRN_ORDERPARAMETER_H

#include "common.h"

struct OrderParameter_;

/// class OrderParameter
///   Consider all permutations of m elements,
///   indexed by contiguous integers in lexigraphical
///   order. This class provides methods which
///   allow one to convert between this indexing
///   and the permutations, along with some convenience
///   functionality (i.e. applying permutations)
class OrderParameter {
public:
  /// OrderParameter
  OrderParameter ( void );

  /// OrderParameter (by index)
  ///   Initialize to the kth permutation of m items
  ///   The permutations are ordered lexicographically
  OrderParameter ( uint64_t m, uint64_t k );

  /// OrderParameter (by permutation)
  ///   Initialize as given permutation
  OrderParameter ( std::vector<uint64_t> const& perm );

  /// assign (by index)
  ///   Initialize to the kth permutation of n items
  ///   The permutations are ordered lexicographically
  void
  assign ( uint64_t m, uint64_t k );

  /// assign (by permutation)
  ///   Initialize as given permutation
  void
  assign ( std::vector<uint64_t> const& perm );

  /// operator ()
  ///   Apply the permutation to the input
  uint64_t
  operator () ( uint64_t i ) const;

  /// inverse
  ///   Apply the inverse permutation to input
  uint64_t
  inverse ( uint64_t i ) const;

  /// permutation ()
  ///   Return the underlying permutation
  std::vector<uint64_t> const&
  permutation ( void ) const;

  /// index ()
  ///   Return the index of the permutation
  uint64_t
  index ( void ) const;

  /// size ()
  ///   Return the number of items
  uint64_t
  size ( void ) const;

  /// stringify
  ///   Return a JSON-style string
  ///    of form [p1, p2, p3, ...]
  std::string
  stringify ( void ) const;

  /// parse
  ///   Initialize from a JSON-style string
  void
  parse ( std::string const& str );

  /// adjacencies ()
  ///   Return the adjacent OrderParameters
  std::vector<OrderParameter>
  adjacencies ( void ) const;

  /// operator <<
  ///   Output debug data to stream
  friend std::ostream& operator << ( std::ostream& stream, OrderParameter const& p );

private:
  std::shared_ptr<OrderParameter_> data_;
  static std::vector<uint64_t> _index_to_tail_rep ( uint64_t index );
  static std::vector<uint64_t> _tail_rep_to_perm ( std::vector<uint64_t> const& tail_rep );
  static std::vector<uint64_t> _perm_to_tail_rep ( std::vector<uint64_t> const& perm );
  static uint64_t _tail_rep_to_index ( std::vector<uint64_t> const& tail_rep );
  /// serialize
  ///   For use with BOOST Serialization library,
  ///   which is used by the cluster-delegator MPI package
  friend class boost::serialization::access;
  template<class Archive>
  void serialize(Archive & ar, const unsigned int version) {
    ar & data_;
  }
};

struct OrderParameter_ {
  std::vector<uint64_t> permute_;
  std::vector<uint64_t> inverse_;
  uint64_t k_;
  uint64_t m_;
  /// serialize
  ///   For use with BOOST Serialization library,
  ///   which is used by the cluster-delegator MPI package
  friend class boost::serialization::access;
  template<class Archive>
  void serialize(Archive & ar, const unsigned int version) {
    ar & permute_;
    ar & inverse_;
    ar & k_;
    ar & m_;
  }
};

#endif
