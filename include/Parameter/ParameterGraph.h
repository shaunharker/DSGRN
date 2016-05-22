/// ParameterGraph.h
/// Shaun Harker
/// 2015-05-24

#ifndef DSGRN_PARAMETERGRAPH_H
#define DSGRN_PARAMETERGRAPH_H

#include "common.h"

#include "Parameter/Network.h"
#include "Parameter/Parameter.h"

struct ParameterGraph_;

class ParameterGraph {
public:
  /// constructor
  ParameterGraph ( void );

  /// ParameterGraph
  ///   Assign a network to the parameter graph
  ///   Search in path for logic .dat files
  ParameterGraph ( Network const& network );

  /// assign
  ///   Assign a network to the parameter graph
  ///   Search in path for logic .dat files
  void
  assign ( Network const& network );

  /// size
  ///   Return the number of parameters
  uint64_t
  size ( void ) const;

  /// parameter
  ///   Return the parameter associated with an index
  Parameter
  parameter ( uint64_t index ) const;

  /// index
  ///   Return the index associated with a parameter
  ///   If the parameter presented is invalid, return -1
  uint64_t
  index ( Parameter const& p ) const;

  /// adjacencies
  ///   Return the adjacent parameter indices to a given parameter index
  std::vector<uint64_t>
  adjacencies ( uint64_t const index ) const;

  /// network
  ///   Return network
  Network const
  network ( void ) const;

  /// fixedordersize
  ///   Return the number of parameters
  ///   for a fixed ordering
  uint64_t
  fixedordersize ( void ) const;

  /// reorderings
  ///   Return of reorderings
  ///   Note: size() = fixedordersize()*reorderings()
  uint64_t
  reorderings ( void ) const;

  /// operator <<
  ///   Stream out information about parameter graph.
  friend std::ostream& operator << ( std::ostream& stream, ParameterGraph const& pg );

private:
  std::shared_ptr<ParameterGraph_> data_;
  uint64_t _factorial ( uint64_t m ) const;
  /// serialize
  ///   For use with BOOST Serialization library,
  ///   which is used by the cluster-delegator MPI package
  friend class boost::serialization::access;
  template<class Archive>
  void serialize(Archive & ar, const unsigned int version) {
    ar & data_;
  }
};

struct ParameterGraph_ {
  Network network_;
  uint64_t size_;
  uint64_t reorderings_;
  uint64_t fixedordersize_;
  std::vector<uint64_t> logic_place_values_;
  std::vector<uint64_t> order_place_values_;
  std::vector<std::vector<std::string>> factors_;
  std::vector<std::unordered_map<std::string,uint64_t>> factors_inv_;
  std::vector<uint64_t> logic_place_bases_;
  std::vector<uint64_t> order_place_bases_;
  /// serialize
  ///   For use with BOOST Serialization library,
  ///   which is used by the cluster-delegator MPI package
  friend class boost::serialization::access;
  template<class Archive>
  void serialize(Archive & ar, const unsigned int version) {
    ar & network_;
    ar & size_;
    ar & reorderings_;
    ar & fixedordersize_;
    ar & logic_place_values_;
    ar & order_place_values_;
    ar & factors_;
    ar & factors_inv_;
    ar & logic_place_bases_;
    ar & order_place_bases_;
  }
};

#endif
