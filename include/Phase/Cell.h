/// Cell.h
/// Shaun Harker
/// 2016-05-25

#ifndef DSGRN_CELL_H
#define DSGRN_CELL_H

// class Cell
class Cell {
public:

  /// Cell
  ///   Default constructor
  Cell ( void );

  /// Cell
  ///   Construct cell as (domain, shape) pair.
  ///   The cell referred to is the one obtained by taking 
  ///   domain and collapsing it (downwards) for each variable
  ///   indicated in shape. The encoding of shape is:
  ///    The ith bit of shape is 0 if the cell is 
  ///      collapsed in the ith dimension
  ///    The ith bit of shape is 1 if i is
  ///      a tangential direction of the cell
  Cell ( Domain const& domain, uint64_t shape );

  /// assign
  ///   Construct cell as (domain, shape) pair.
  ///   The cell referred to is the one obtained by taking 
  ///   domain and collapsing it (downwards) for each variable
  ///   indicated in shape. The encoding of shape is:
  ///    The ith bit of shape is 0 if the cell is 
  ///      collapsed in the ith dimension
  ///    The ith bit of shape is 1 if i is
  ///      a tangential direction of the cell
  void
  assign ( Domain const& domain, uint64_t shape );

  /// collapse
  ///   Given a list of pairs of (variable, direction), collapse the cell in
  ///   each variable in the indicated direction (false means left, true means right)
  ///   If the cell is already collapsed in some direction, throw an error
  ///   If we attempt to collapse past the last threshold, return the default Cell
  Cell
  collapse ( std::vector<std::pair<int,bool>> const& collapses ) const;

  /// expand
  ///   Given a list of pairs of (variable, direction), expand the cell in
  ///   each variable in the indicated direction (false means left, true means right)
  ///   If the cell already has extent in some direction, throw an error.
  ///   If we attempt to expand out of the positive orthant, return the default Cell
  Cell
  expand ( std::vector<std::pair<int,bool>> const& expansions ) const;

  /// index
  ///   Return an index defined by the formula
  ///     domain_index << D | shape
  uint64_t
  index ( void ) const;

  /// operator bool
  ///   Return true if initialized; return false if default constructed
  operator bool () const;

  /// operator ==
  ///   Check if two cells are equal
  bool
  operator == ( Cell const& rhs ) const;

  /// operator <<
  ///   Output to stream
  friend std::ostream& operator << ( std::ostream& stream, Cell const& dom );

private:
  Domain domain_;
  uint64_t shape_;
  /// serialize
  ///   For use with BOOST Serialization library,
  ///   which is used by the cluster-delegator MPI package
  friend class boost::serialization::access;
  template<class Archive>
  void serialize(Archive & ar, const unsigned int version) {
    ar & domain_;
    ar & shape_;
  }
};

std::size_t hash_value (Cell const& cell);

#endif
