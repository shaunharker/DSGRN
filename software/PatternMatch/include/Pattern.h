/// Pattern.h
/// Shaun Harker and Bree Cummins
/// 2016-03-21

#ifndef PATTERN_H
#define PATTERN_H

#include "DSGRN.h"
#include "common.h"

struct Pattern_;

class Pattern {
public:
  /// Pattern
  ///   Default constructor
  Pattern ( void );

  /// Pattern
  ///   Construct from a poset and event data
  ///   Inputs: "poset" is the poset of min/max events
  ///           "events" is a map from poset elements to event
  ///                    variable indexing. (Note: it does have
  ///                    min/max data.)
  ///           "initial_label" gives the initial state before
  ///                    any events. Further details below.
  ///           "dimension" is the number of variables
  ///   The label is a 64 bit word with bits interpreted as follows:
  ///    bit i+D     bit i   interpretation
  ///         0        1    ith variable decreasing  
  ///         1        0    ith variable increasing
  ///   Note the limitation to 32 dimensions. Here D is the number of
  ///   variables.
  Pattern ( Poset const& poset, 
            std::unordered_map<uint64_t,uint64_t> const& events,
            uint64_t initial_label,
            uint64_t dimension );

  /// assign
  ///   Construct from a poset and event data
  void
  assign ( Poset const& poset, 
           std::unordered_map<uint64_t,uint64_t> const& events,
           uint64_t initial_label, 
           uint64_t dimension );

  /// label
  ///   Return the label corresponding to the initial state
  ///   The label is a 64 bit word with bits interpreted as follows:
  ///    bit i+D     bit i   interpretation
  ///         0        1    ith variable decreasing  
  ///         1        0    ith variable increasing
  ///         0        0    ith variable can either increase or decrease
  ///   Note the limitation to 32 dimensions. Here D is the number of
  ///   dimensions.
  uint64_t
  label ( void ) const;

  /// dimension
  ///   Return number of dimensions
  uint64_t
  dimension ( void ) const;

  /// poset
  ///   Return underlying poset
  Poset const& 
  poset ( void ) const;

  /// event
  ///   Return event variable associated with vertex
  uint64_t
  event ( uint64_t v ) const;

private:
  std::shared_ptr<Pattern_> data_;
  /// serialize
  ///   For use with BOOST Serialization library,
  ///   which is used by the cluster-delegator MPI package
  friend class boost::serialization::access;
  template<class Archive>
  void serialize(Archive & ar, const unsigned int version) {
    ar & data_;
  }  
};

struct Pattern_ {
  Poset poset_;
  std::unordered_map<uint64_t,uint64_t> events_;
  uint64_t label_;
  uint64_t dimension_;
  /// serialize
  ///   For use with BOOST Serialization library,
  ///   which is used by the cluster-delegator MPI package
  friend class boost::serialization::access;
  template<class Archive>
  void serialize(Archive & ar, const unsigned int version) {
    ar & poset_;
    ar & events_;
    ar & label_;
    ar & dimension_;
  }
};

#endif
