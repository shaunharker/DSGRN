/// Annotation.h
/// Shaun Harker
/// 2015-05-15

#ifndef DSGRN_ANNOTATION_H
#define DSGRN_ANNOTATION_H

#include "common.h"

struct Annotation_;

/// Annotation
///   A lightweight string container
///   used to annotate Morse Graphs
class Annotation {
public:
  /// iterator
  ///   STL-style iterator
  typedef std::vector<std::string>::const_iterator iterator;

  /// constructor
  Annotation ( void );

  /// size
  ///   Return the number of annotations
  uint64_t 
  size ( void ) const;

  /// begin
  ///   Return start iterator
  iterator
  begin ( void ) const;

  /// end
  ///   Return end iterator
  iterator 
  end ( void ) const;

  /// operator []
  ///   Random access to contents
  std::string const&
  operator [] ( uint64_t i ) const;

  /// append
  ///   Add more annotations
  void
  append ( std::string const& label );

  /// stringify
  ///   Return a JSON description
  std::string
  stringify ( void ) const;

  /// parse
  ///   Initialize from a JSON description
  void
  parse ( std::string const& str );

  /// operator <<
  ///   Output list of annotations
  friend std::ostream& operator << ( std::ostream& stream, Annotation const& a );
private:
  std::shared_ptr<Annotation_> data_;

  /// serialize
  ///   For use with BOOST Serialization library,
  ///   which is used by the cluster-delegator MPI package
  friend class boost::serialization::access;
  template<class Archive>
  void serialize(Archive & ar, const unsigned int version) {
    ar & data_;
  }
};

struct Annotation_ {
  std::vector<std::string> annotations_;
  /// serialize
  ///   For use with BOOST Serialization library,
  ///   which is used by the cluster-delegator MPI package
  friend class boost::serialization::access;
  template<class Archive>
  void serialize(Archive & ar, const unsigned int version) {
    ar & annotations_;
  }
};
#endif
