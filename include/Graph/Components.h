/// Components.h
/// Shaun Harker
/// 2015-05-24

#ifndef DSGRN_COMPONENTS_H
#define DSGRN_COMPONENTS_H

#include "common.h"

#include "boost/function.hpp"
#include "boost/bind.hpp"
#include "boost/range/sub_range.hpp"
#include "boost/range/iterator_range.hpp"
#include "boost/iterator/counting_iterator.hpp"
#include "boost/iterator/transform_iterator.hpp"

typedef boost::sub_range < std::vector<uint64_t> > Component;

struct Components_;

class Components {
public:
  typedef boost::function < Component ( int64_t ) > Functor;
  typedef boost::transform_iterator<Functor, boost::counting_iterator<int64_t> > iterator;
  typedef boost::iterator_range<iterator> ComponentContainer;

  /// Components
  Components ( void );

  /// Components
  ///   Assign data
  Components ( std::vector<uint64_t> const& vertices,
               std::vector<bool> const& components,
               std::vector<bool> const& recurrent );

  /// assign
  ///   Assign data
  void
  assign ( std::vector<uint64_t> const& vertices,
           std::vector<bool> const& components,
           std::vector<bool> const& recurrent );

  /// begin
  ///   beginning Component iterator
  iterator
  begin ( void ) const;

  /// end
  ///   one-past-the-end Component iterator
  iterator
  end ( void ) const;

  /// size
  ///   Returns number of components
  uint64_t
  size ( void ) const;

  /// operator []
  ///   Random access to components
  Component
  operator [] ( uint64_t i ) const;

  /// recurrentComponents
  ///   Return a container for recurrent components
  ///   that supplies operator [], begin, end, size
  ComponentContainer const& 
  recurrentComponents ( void ) const;

  /// isRecurrent
  ///   Return "true" if ith strongly connected component is recurrent
  bool 
  isRecurrent ( int64_t i ) const;

  /// whichComponent
  ///   Given a vertex determines which 
  ///   strong component it is in
  uint64_t
  whichComponent ( uint64_t i ) const;
  
  /// operator <<
  ///   Output to stream
  friend std::ostream& operator << ( std::ostream& stream, Components const& c );

private: 
  std::shared_ptr<Components_> data_;

  /// serialize: save and load
  ///   For use with BOOST Serialization library,
  ///   which is used by the cluster-delegator MPI package
  friend class boost::serialization::access;
  template<class Archive>
  void save(Archive & ar, const unsigned int version) const;
  template<class Archive>
  void load(Archive & ar, const unsigned int version);

  BOOST_SERIALIZATION_SPLIT_MEMBER()
};

struct Components_ {
  // Gives reverse topological sort when flattened
  std::vector<uint64_t> vertices_;
  std::vector<uint64_t> which_component_;
  std::vector<bool> components_;
  std::vector<bool> recurrent_;
  std::vector<uint64_t> component_select_;
  std::vector<uint64_t> recurrent_select_;
  Components::ComponentContainer component_container_;
  Components::ComponentContainer recurrent_container_;

  /// component
  ///  Return the ith component
  Component 
  _component ( int64_t i );

  /// recurrentComponent
  ///    Return the "rank"th recurrent component
  Component 
  _recurrentComponent ( int64_t rank );

  /// serialize: save and load
  ///   For use with BOOST Serialization library,
  ///   which is used by the cluster-delegator MPI package
  friend class boost::serialization::access;
  template<class Archive>
  void save(Archive & ar, const unsigned int version) const {
    ar << vertices_;
    ar << components_;
    ar << recurrent_;
  }

  template<class Archive>
  void load(Archive & ar, const unsigned int version) {
    ar >> vertices_;
    ar >> components_;
    ar >> recurrent_;
  }
  BOOST_SERIALIZATION_SPLIT_MEMBER()
};

template<class Archive> void Components::
save(Archive & ar, const unsigned int version) const {
  ar << data_;
}

template<class Archive> void Components::
load(Archive & ar, const unsigned int version) {
  ar >> data_;
  assign ( data_ -> vertices_, data_ -> components_, data_ -> recurrent_ );
}

#endif
