/// Components.h
/// Shaun Harker
/// 2015-05-24

#ifndef DSGRN_COMPONENTS_H
#define DSGRN_COMPONENTS_H

#include "common.h"

struct Components_;

class Components {
public:
  typedef std::vector<std::shared_ptr<std::vector<uint64_t>>>::const_iterator iterator;

  /// Components
  Components ( void );

  /// Components
  ///   Assign data
  Components ( std::vector<std::shared_ptr<std::vector<uint64_t>>> const& SCCs );

  /// assign
  ///   Assign data
  void
  assign ( std::vector<std::shared_ptr<std::vector<uint64_t>>> const& SCCs );

  /// size
  ///   Returns number of vertices
  iterator
  begin ( void ) const;

  /// size
  ///   Returns number of vertices
  iterator
  end ( void ) const;

  /// size
  ///   Returns number of vertices
  uint64_t
  size ( void ) const;

  /// operator []
  ///   Random access to components
  std::shared_ptr<std::vector<uint64_t>>
  operator [] ( uint64_t i ) const;

  /// whichStrong
  ///   Given a vertex determines which 
  ///   strong component it is in
  uint64_t
  whichComponent ( uint64_t i ) const;
  
  /// operator <<
  ///   Output to stream
  friend std::ostream& operator << ( std::ostream& stream, Components const& c );

private: 
  std::shared_ptr<Components_> data_;
  /// serialize
  ///   For use with BOOST Serialization library,
  ///   which is used by the cluster-delegator MPI package
  friend class boost::serialization::access;
  template<class Archive>
  void serialize(Archive & ar, const unsigned int version) {
    ar & data_;
  }
};

struct Components_ {
  // Gives reverse topological sort when flattened
  std::vector<std::shared_ptr<std::vector<uint64_t>>> components_;
  std::unordered_map<uint64_t, uint64_t> which_component_;
  /// serialize
  ///   For use with BOOST Serialization library,
  ///   which is used by the cluster-delegator MPI package
  friend class boost::serialization::access;
  template<class Archive>
  void serialize(Archive & ar, const unsigned int version) {
    ar & components_;
    ar & which_component_;
  }
};

#endif
