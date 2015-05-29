/// Components.h
/// Shaun Harker
/// 2015-05-24

#ifndef DSGRN_COMPONENTS_H
#define DSGRN_COMPONENTS_H

#include <vector>
#include <memory>
#include <cstdlib>

class Components {
public:
  typedef std::vector<std::shared_ptr<std::vector<uint64_t>>>::const_iterator iterator;

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
  
private: 
  // Gives reverse topological sort when flattened
  std::vector<std::shared_ptr<std::vector<uint64_t>>> components_;
  std::unordered_map<uint64_t, uint64_t> which_component_;
};

inline void Components::
assign ( std::vector<std::shared_ptr<std::vector<uint64_t>>> const& SCCs ) {
  which_component_ . clear ();
  components_ = SCCs;
  for ( uint64_t comp_num = 0; comp_num < components_ . size (); ++ comp_num ) {
    for ( uint64_t v : * components_ [ comp_num ] ) {
      which_component_ [ v ] = comp_num;
    }
  }
}

inline Components::iterator Components::
begin ( void ) const {
  return components_ . begin ();
}

inline Components::iterator Components::
end ( void ) const {
  return components_ . end ();
}

inline uint64_t Components::
size ( void ) const {
  return components_ . size ();
}

inline std::shared_ptr<std::vector<uint64_t>> Components::
operator [] ( uint64_t i ) const {
  return components_ [ i ];
}

inline uint64_t Components::
whichComponent ( uint64_t i ) const {
  auto it = which_component_ . find ( i );
  if ( it == which_component_  . end () ) return components_ . size ();
  return it -> second;
}

#endif
