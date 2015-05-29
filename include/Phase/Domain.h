/// Domain.h
/// Shaun Harker
/// 2015-05-24

#ifndef DSGRN_DOMAIN_H
#define DSGRN_DOMAIN_H

#include <vector>
#include <cstdlib>

/// class Domain
///   This class allows for n-tuples with non-negative
///   entries less than specified limits (set in the
///   the constructor). It provides operator () to access
///   the components as well as increment operators. The
///   increment operator tries to increment the 0th
///   position; if limit is reached it resets to 0 and
///   carried to the next digit, and so forth. Incrementing
///   the largest domain within the limits results in an
///   invalid domain (and isValid stops returning true).
class Domain {
public:
  /// Domain
  ///   Construct domain object as
  ///   (0,0,...,0) in 
  ///   {0,1,..,limits[0]-1}x...x{0,1,..,limits[D]-1}
  Domain ( std::vector<uint64_t> const& limits );

  /// operator []
  ///   Return dth component
  uint64_t 
  operator [] ( uint64_t d ) const;

  /// operator ++ (preincrement)
  ///   Advance through domain traversal pattern
  Domain & 
  operator ++ ( void );

  /// operator ++ (postincrement)
  ///   Advance through domain traversal pattern
  ///   but return copy of unadvanced domain
  Domain 
  operator ++ ( int );

  /// size
  ///   Return number of dimensions
  uint64_t 
  size ( void ) const;

  /// index
  ///   Return traversal index of domain
  uint64_t 
  index ( void ) const;

  /// setIndex 
  ///   Set the domain by index
  void
  setIndex ( uint64_t i );

  /// left
  ///   Return domain index to the left in dimension d
  ///   (If there is none, behavior is undefined)
  uint64_t
  left ( uint64_t d ) const;

  /// right
  ///   Return domain index to the right in dimension d
  ///   (If there is none, behavior is undefined)
  uint64_t
  right ( uint64_t d ) const;

  /// right
  /// isMin
  ///   Return true if dth component 
  ///   is 0 (i.e. is minimal)
  bool 
  isMin ( uint64_t d ) const;

  /// isMax
  ///   Return true if dth component 
  ///   is limits[d]-1 (i.e. is maximal)
  bool 
  isMax ( uint64_t d ) const;

  /// isValid
  ///   Return true if a valid domain.
  ///   (For use with ++, so one-past-the-end is not valid)
  bool
  isValid ( void ) const;

private:
  std::vector<uint64_t> data_;
  std::vector<uint64_t> limits_; 
  std::vector<uint64_t> offset_; 
  uint64_t index_;
  uint64_t max_;
  uint64_t D_;
};

inline Domain::
Domain ( std::vector<uint64_t> const& limits ) 
  : limits_(limits) {
  index_ = 0;
  D_ = limits_ . size ();
  data_ . resize ( D_, 0 );
  offset_ . resize ( D_ );
  max_ = 1;
  for ( uint64_t d = 0; d < D_; ++ d ) {
    offset_[d] = max_;
    max_ *= limits_ [ d ];
  }
}

inline uint64_t Domain::
operator [] ( uint64_t d ) const {
  return data_[d];
}

inline Domain & Domain::
operator ++ ( void ) {
  for ( uint64_t d = 0; d < D_; ++ d ) {
    if ( ++ data_ [ d ] < limits_ [ d ] ) break;
    data_ [ d ] = 0;
  }
  ++ index_;
  return *this;
}

inline Domain Domain::
operator ++ ( int ) {
  Domain result = *this;
  ++ (*this);
  return result;
}

inline uint64_t Domain::
size ( void ) const {
  return limits_ . size ();
}

inline uint64_t Domain::
index ( void ) const {
  return index_;
}

inline void Domain::
setIndex ( uint64_t i ) {
  index_ = i;
  for ( uint64_t d = 0; d < D_; ++ d ) {
    data_ [ d ] = i % limits_ [ d ];
    i /= limits_ [ d ];
  }
}

inline uint64_t Domain::
left ( uint64_t d ) const {
  return index_ - offset_[d];
}

inline uint64_t Domain::
right ( uint64_t d ) const {
  return index_ + offset_[d];
}

inline bool Domain::
isMin ( uint64_t d ) const {
  return data_[d] == 0;
}

inline bool Domain::
isMax ( uint64_t d ) const {
  return data_[d] == limits_[d]-1;
}

inline bool Domain::
isValid ( void ) const {
  return index_ < max_;
}

#endif
