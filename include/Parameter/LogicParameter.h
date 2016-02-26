/// LogicParameter.h
/// Shaun Harker
/// 2015-05-24

#ifndef DSGRN_LOGICPARAMETER_H
#define DSGRN_LOGICPARAMETER_H

#include "common.h"

struct LogicParameter_;

class LogicParameter {
public:
  /// LogicParameter
  LogicParameter ( void );

  /// LogicParameter
  LogicParameter ( uint64_t n, uint64_t m, std::string const& hex );

  /// assign
  ///   Assign data
  void
  assign ( uint64_t n, uint64_t m, std::string const& hex );

  /// operator ()
  ///   Return true if input combination drives the output
  ///   to the right, and false if it drives it to the left
  bool
  operator () ( std::vector<bool> const& input_combination, uint64_t output ) const;

  /// operator ()
  ///   Version of operator () where bit has already been computed
  bool
  operator () ( uint64_t bit ) const;

  /// bin
  ///   Return bin number corresponding to input combination
  ///   (uses uint64_t, assumes there are 64 or fewer inputs)
  uint64_t
  bin ( uint64_t input_combination ) const;

  /// stringify
  ///   Return a JSON-style string
  ///    of form [n,m,"hex"]
  std::string
  stringify ( void ) const;

  /// parse
  ///   Initialize from a JSON-style string
  void
  parse ( std::string const& str );

  /// hex ()
  ///   Return the hex code of the LogicParameter
  std::string const &
  hex ( void ) const;

  /// adjacencies ()
  ///   Return the adjacent LogicParameters
  std::vector<LogicParameter>
  adjacencies ( void ) const;

  /// operator <<
  ///   Output debug data to stream
  friend std::ostream& operator << ( std::ostream& stream, LogicParameter const& p );

private:
  std::shared_ptr<LogicParameter_> data_;
  /// serialize
  ///   For use with BOOST Serialization library,
  ///   which is used by the cluster-delegator MPI package
  friend class boost::serialization::access;
  template<class Archive>
  void serialize(Archive & ar, const unsigned int version) {
    ar & data_;
  }
};

struct LogicParameter_ {
  std::string hex_;
  std::vector<bool> comp_;
  uint64_t n_;
  uint64_t m_;
  /// serialize
  ///   For use with BOOST Serialization library,
  ///   which is used by the cluster-delegator MPI package
  friend class boost::serialization::access;
  template<class Archive>
  void serialize(Archive & ar, const unsigned int version) {
    ar & hex_;
    ar & comp_;
    ar & n_;
    ar & m_;
  }
};
#endif
