/// Parameter.h
/// Shaun Harker
/// 2015-05-24

#ifndef DSGRN_PARAMETER_H
#define DSGRN_PARAMETER_H

#include "common.h"

#include "Phase/Domain.h"
#include "Parameter/Network.h"
#include "Parameter/LogicParameter.h"
#include "Parameter/OrderParameter.h"

struct Parameter_;

class Parameter {
public:
  /// Parameter
  Parameter ( void );

  /// Parameter
  ///   Assign data to parameter
  Parameter ( std::vector<LogicParameter> const& logic,
              std::vector<OrderParameter> const& order, 
              Network const& network );

  /// Parameter (network-only)
  ///   Assign network to parameter.
  ///   logic and order left uninitialized.
  Parameter ( Network const& network );

  /// assign
  ///   Assign data to parameter
  void
  assign ( std::vector<LogicParameter> const& logic,
           std::vector<OrderParameter> const& order, 
           Network const& network );

  /// assign (network-only)
  ///   Assign network to parameter.
  ///   logic and order left uninitialized.
  void
  assign ( Network const& network );

  /// attracting
  ///   Return true if domain is attracting.
  ///   This is true iff none of the walls of
  ///   are absorbing.
  bool 
  attracting ( Domain const& dom ) const;

  /// absorbing
  ///   Return true if wall is absorbing
  ///   The wall is described by a domain dom,
  ///   a dimension collapse_dim, and a direction. 
  ///   The direction is either -1 or +1, signifying
  ///   the left wall or the right wall with 
  ///   collapse dimension collapse_dim.
  bool
  absorbing ( Domain const& dom, int collapse_dim, int direction ) const;

  /// network
  ///   Return network
  Network const
  network ( void ) const;

  /// stringify
  ///   Return a JSON description
  std::string
  stringify ( void ) const;

  /// parse
  ///   Initialize from a JSON description
  void
  parse ( std::string const& str );

  /// inequalities
  ///    Output a list of inequalities correspond to the parameter
  std::string
  inequalities ( void ) const;

  /// operator <<
  ///   Output debug data to stream
  friend std::ostream& operator << ( std::ostream& stream, Parameter const& p );

private:
  std::shared_ptr<Parameter_> data_;
  /// serialize
  ///   For use with BOOST Serialization library,
  ///   which is used by the cluster-delegator MPI package
  friend class boost::serialization::access;
  template<class Archive>
  void serialize(Archive & ar, const unsigned int version) {
    ar & data_;
  }
};

struct Parameter_ {
  std::vector<LogicParameter> logic_;
  std::vector<OrderParameter> order_;
  Network network_;
  /// serialize
  ///   For use with BOOST Serialization library,
  ///   which is used by the cluster-delegator MPI package
  friend class boost::serialization::access;
  template<class Archive>
  void serialize(Archive & ar, const unsigned int version) {
    ar & logic_;
    ar & order_;
    ar & network_;
  }
};
#endif
