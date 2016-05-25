/// Parameter.h
/// Shaun Harker
/// 2015-05-24

#ifndef DSGRN_PARAMETER_H
#define DSGRN_PARAMETER_H

#include "common.h"

#include "Phase/Domain.h"
#include "Phase/Cell.h"
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
  absorbing ( Domain const& dom, uint64_t collapse_dim, int direction ) const;

  /// regulator
  ///   Return the variable being regulated on the threshold
  ///   indicated, where "variable" is the normal to the threshold
  ///   and "threshold" indicates which threshold, counting from
  ///   lower to highest, starting from 0.
  uint64_t
  regulator ( uint64_t variable, uint64_t threshold ) const;

  /// labelling
  ///   Returns a data structure representing if each wall is an
  ///   entrance or absorbing. The data structure is
  ///   currently limited to handling networks with at most 32 nodes.
  ///   The result is a vector of uint64_t. The vector is indexed by
  ///   domain index. For domain indexing, see Domain.h.
  ///   Each uint64_t entry has bits corresponding to each of the
  ///   2*d walls in the following pattern:
  ///     (from least significant to most significant bits)
  ///     left-0, left-1, left-2, ..., left-(d-1),
  ///     right-0, right-1, ... right(d-1)
  ///   A bit of 0 means entrance and 1 means absorbing.
  std::vector<uint64_t>
  labelling ( void ) const;

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
  ///    Output a list of inequalities corresponding to the parameter node.
  ///    We output the list in a format which is compatible with Mathematica's syntax,
  ///    encapsulated in a JSON string. In particular, the format is as follows:
  ///    {"inequalities" : "mathematica-parsable-string", "variables" : "mathematica-parsable-string"}
  std::string
  inequalities ( void ) const;

  /// logic ()
  ///   Return the logic of the parameter
  std::vector<LogicParameter> const &
  logic ( void ) const;

  /// order ()
  ///   Return the order of the parameter
  std::vector<OrderParameter> const &
  order ( void ) const;

  ////////////////////////////////
  // Routines used by TileGraph //
  ////////////////////////////////

  /// snoussi
  ///   Return true if a given cell is a snoussi cell
  ///   A Snoussi cell is one for which the mapping between 
  ///   threshold normals and threshold regulated variables
  ///   is a bijection
  bool
  snoussi ( Cell const& cell ) const;

  /// hyperoctahedral
  ///   Given: a Snoussi cell (otherwise undefined behavior)
  ///   Output: A map from each normal direction x to "cell" to
  ///           a pair (int, bool) such that
  ///           For each normal direction x:
  ///             Let y be the variable regulated by x     
  ///             If the flow in dimension y switches from
  ///               negative to positive as we pass threshold x, 
  ///                   result[x] = (y, true)
  ///             If the flow in dimension y switches from
  ///               positive to negative as we pass threshold x,
  ///                   result[x] = (y, false)
  ///             If the flow in dimension y does not switch 
  ///               as we pass threshold x,
  ///                   result[x] = (-1, false)
  std::unordered_map<uint64_t, std::pair<uint64_t, bool>>
  hyperoctahedral ( Cell const& cell ) const;

  /// absorbing 
  ///   Given a cell and a tangential dimension
  ///   for which the flow direction is well defined, 
  ///   return the flow direction. (If not well-defined, 
  //    undefined behavior.)
  bool
  absorbing ( Cell const& cell, uint64_t collapse_dim, int direction ) const;

  /// attracting
  ///   Return true if cell is attracting.
  ///   This is true iff none of the walls of
  ///   are absorbing. (If not well-defined, undef. behavior)
  bool
  attracting ( Cell const& cell ) const;

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
