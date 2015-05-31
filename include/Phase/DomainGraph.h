/// DomainGraph.h
/// Shaun Harker
/// 2015-05-24

#ifndef DSGRN_DOMAINGRAPH_H
#define DSGRN_DOMAINGRAPH_H

#include "common.h"
#include "Parameter/Parameter.h"
#include "Graph/Digraph.h"
#include "Dynamics/Annotation.h"

struct DomainGraph_;

class DomainGraph {
public:
  /// constructor
  DomainGraph ( void );

  /// DomainGraph
  ///   Construct based on parameter and network
  DomainGraph ( Parameter const parameter );

  /// assign
  ///   Construct based on parameter and network
  void
  assign ( Parameter const parameter );

  /// digraph
  ///   Return underlying digraph
  Digraph const
  digraph ( void ) const;  

  /// annotate
  Annotation const
  annotate ( std::vector<uint64_t> const& vertices ) const;

  /// operator <<
  ///   Emit data to stream in graphviz format
  friend std::ostream& operator << ( std::ostream& stream, DomainGraph const& dg );

private:
  std::shared_ptr<DomainGraph_> data_;
  /// serialize
  ///   For use with BOOST Serialization library,
  ///   which is used by the cluster-delegator MPI package
  friend class boost::serialization::access;
  template<class Archive>
  void serialize(Archive & ar, const unsigned int version) {
    ar & data_;
  }
};

struct DomainGraph_ {
  Digraph digraph_;
  Parameter parameter_;
  /// serialize
  ///   For use with BOOST Serialization library,
  ///   which is used by the cluster-delegator MPI package
  friend class boost::serialization::access;
  template<class Archive>
  void serialize(Archive & ar, const unsigned int version) {
    ar & digraph_;
    ar & parameter_;
  }
};

#endif
