/// Cell.hpp
/// Shaun Harker
/// 2016-05-25

#ifndef DSGRN_CELL_HPP
#define DSGRN_CELL_HPP


INLINE_IF_HEADER_ONLY Cell::
Cell ( void ) {}

INLINE_IF_HEADER_ONLY Cell::
Cell ( Domain const& domain, uint64_t shape ) {
  assign ( domain, shape );
}

INLINE_IF_HEADER_ONLY void Cell::
assign ( Domain const& domain, uint64_t shape ) {
  domain_ = domain;
  shape_ = shape;
}

INLINE_IF_HEADER_ONLY Cell Cell::
collapse ( std::vector<std::pair<int,bool>> const& collapses ) const {
  Cell result = *this;
  for ( auto const& collapse : collapses ) {
    int collapse_dimension = collapse.first;
    bool collapse_direction = collapse.second;
    uint64_t collapse_bit = 1 << collapse_dimension;
    // Make sure this is valid
    if ( result . shape_ & collapse_bit == 0 ) {
      // Throw if attempting impossible collapse
      throw std::runtime_error("Cell::collapse. Impossible collapse.");
    }
    result . shape_ ^= collapse_bit;
    // If collapsing right, move domain
    if ( collapse_direction == true ) { 
      // Return default constructed cell if attempting to collapse in unbounded direction
      if ( result . domain_ . isMax ( collapse_dimension ) ) {
        return Cell (); 
      }
      result . domain_ . setIndex ( result . domain_ . right ( collapse_dimension ) );
    }
  }
  return result;
}

INLINE_IF_HEADER_ONLY Cell Cell::
expand ( std::vector<std::pair<int,bool>> const& expansions ) const {
  Cell result = *this;
  for ( auto const& expansion : expansions ) {
    int expansion_dimension = expansion.first;
    bool expansion_direction = expansion.second;
    uint64_t expansion_bit = 1 << expansion_dimension;
    // Make sure this is valid
    if ( result . shape_ & collapse_bit ) {
      // Throw if attempting impossible expansion
      throw std::runtime_error("Cell::expand. Impossible expansion.");
    }
    result . shape_ ^= expansion_bit;
    // If expanding left, move domain
    if ( expansion_direction == false ) { 
      // Return default constructed cell if attempting to expand beyond lower bound
      if ( result . domain . isMin ( collapse_dimension ) ) {
        return Cell (); 
      }
      result . domain_ . setIndex ( result . domain_ . left ( collapse_dimension ) );
    }
  }
  return result;
}


INLINE_IF_HEADER_ONLY uint64_t Cell::
index ( void ) const {
  return domain_ . index() << domain_ . size () + shape_;
}


INLINE_IF_HEADER_ONLY Cell::
operator bool () const {
  return domain_ . size () > 0;
}


INLINE_IF_HEADER_ONLY bool Cell::
operator == ( Cell const& rhs ) const {
  return index() == rhs . index();
}


INLINE_IF_HEADER_ONLY std::ostream& 
operator << ( std::ostream& stream, Cell const& cell ) {
  uint64_t D = cell . domain_ . size ();
  for ( uint64_t d = 0; d < D; ++ d ) {
    uint64_t left = cell . domain_ [ d ];
    if ( d > 0 ) stream << "x"
    if ( shape_ & (1 << i) ) {
      // Has extend
      stream << "[" << left << ", " << left+1 << "]";
    } else {
      stream << "[" << left << ", " << left << "]";
    }
  } 
  return stream;
}

INLINE_IF_HEADER_ONLY std::size_t 
hash_value (Cell const& cell) {
  return cell . index ();
}

#endif
