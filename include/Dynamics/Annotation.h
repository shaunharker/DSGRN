/// Annotation.h
/// Shaun Harker
/// 2015-05-15

#ifndef DSGRN_ANNOTATION_H
#define DSGRN_ANNOTATION_H

#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include <cstdlib>

#include "boost/serialization/serialization.hpp"
#include "boost/serialization/string.hpp"

/// Annotation
///   A lightweight string container
///   used to annotate Morse Graphs
class Annotation {
public:

  /// iterator
  ///   STL-style iterator
  typedef std::vector<std::string>::const_iterator iterator;

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


inline uint64_t Annotation::
size ( void ) const {
  return annotations_ . size ();
}

inline Annotation::iterator Annotation::
begin ( void ) const {
  return annotations_ . begin ();
}

inline Annotation::iterator Annotation::
end ( void ) const {
  return annotations_ . end ();
}

inline void Annotation::
append ( std::string const& label ) {
  annotations_ . push_back ( label );
}

inline std::string Annotation::
stringify ( void ) const {
  std::stringstream ss;
  ss << "[";
  bool first = true;
  for ( std::string const& s : annotations_ ) {
    if ( first ) first = false; else ss << ",";
    ss << "\"" << s << "\"";
  }
  ss << "]";
  return ss . str ();
}

inline void Annotation::
parse ( std::string const& str ) {
  std::shared_ptr<JSON::Array> array = JSON::toArray(JSON::parse(str));
  annotations_ . clear ();
  uint64_t N = array -> size ();
  for ( uint64_t v = 0; v < N; ++ v ) {
    std::shared_ptr<JSON::String> js = JSON::toString((*array)[v]);
    annotations_ . push_back ( *js );
  } 
}

inline std::ostream& operator << ( std::ostream& stream, Annotation const& a ) {
  stream << "{";
  bool first = true;
  for ( auto x : a . annotations_ ) {
    if ( first ) first = false; else stream << ", ";
    stream << "\"" << x << "\"";
  }
  stream << "}";
  return stream;
}

#endif
