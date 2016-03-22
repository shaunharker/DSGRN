/// Pattern.h
/// Shaun Harker and Bree Cummins
/// 2016-03-21

#include "Pattern.h"

Pattern::
Pattern ( void ) {
  data_ . reset ( new Pattern_ );
}

Pattern::
Pattern ( Poset const& poset, 
          std::unordered_map<uint64_t,uint64_t> const& events,
          uint64_t initial_label,
          uint64_t dimension ) {
  assign ( poset, events, initial_label, dimension );
}

void Pattern::
assign ( Poset const& poset, 
         std::unordered_map<uint64_t,uint64_t> const& events, 
         uint64_t initial_label,
         uint64_t dimension ) {
  data_ . reset ( new Pattern_ );
  data_ -> poset_ = poset;
  data_ -> events_ = events;
  data_ -> label_ = initial_label;
  data_ -> dimension_ = dimension;
}

uint64_t Pattern::
label ( void ) const {
  return data_ -> label_;
}

uint64_t Pattern::
dimension ( void ) const {
  return data_ -> dimension_;
}

Poset const& Pattern::
poset ( void ) const {
  return data_ -> poset_;
}

uint64_t Pattern::
event ( uint64_t v ) const {
  return data_ -> events_ [ v ];
}
