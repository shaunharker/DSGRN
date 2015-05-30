/// Idiom.h
/// Shaun Harker
/// 2015-05-29

#ifndef DSGRN_IDIOM_H
#define DSGRN_IDIOM_H

#include <iostream>
#include <memory>
#include "boost/serialization/serialization.hpp"

/// Idiom
///
///  We wrap our classes using a pointer-to-implementation "PIMPL" idiom.
///  Our conventions for this are as follows.
///  For each user class, there is an implementation class. The
///  name of the implementation class is the same as the name of
///  the user class except it is suffixed by an underscore. For
///  example, MorseGraph has an implementation class MorseGraph_.
///  We achieve PIMPL by inheriting a user class X from a template 
///  class Interface<X_>. This class holds an std::shared_ptr to
///  the implementation class and provides definitions for 
///  operator << and serialize automatically. It is also 
///  default constructible, leading to the requirement that all
///  implementation classes using this idiom are also default
///  constructible. The user class X is defined using the macro
///  INTERFACE(X). This macro automatically forward declares X_,
///  and declares X as an inherited struct of Interface<X_>.
///  The definitions of all other user class methods are accomplished
///  by inserting the macro IMPLEMENT(method) at the of each declaration
///  before the semicolon, e.g.
///   
///     std::string f ( int a, int b ) const IMPLEMENT(f(a,b));
///
///  This has the effect of making the declaration into a definition, 
///  which calls the same-named method through the impl_ pointer and
///  returns its result.
///  To declare constructors we use the CONSTRUCT macro which works
///  slightly differently since we must accommodate the underscore.
///  Example:
///   X(int a) CONSTRUCT(X_(a));
///  This resets the internal shared pointer to point at a newly
///  constructed object.
///  If the method returns a reference to itself, use the maco
///  CHAIN. And if all else fails, just write the code yourself!

/// Remarks on idiom. 
///   The point of wrapping everything in shared pointers with these
///   passthrough wrappers is to make objects lightweight while still 
///   enjoying value semantics. Best practices are to not return
///   const& or & of these objects as return type but rather return
///   their lightweight copies, which up the reference count.
///   For inputs, it is less relevant since we can make the assumption
///   that the calling context remains in scope so passing by
///   const& is OK.
///   Under no circumstances use a const& member variable!

#define IMPLEMENT(method) { return impl_ -> method; }
#define CONSTRUCT(object) { impl_ . reset ( new object ); }
#define CHAIN(method) { impl_ -> method; return *this; }
#define INTERFACE(X) class X ## _ ; struct X : public Pimpl< X ## _ >

template<class T> 
class Pimpl {
protected: 
  std::shared_ptr<T> impl_; 
public: 
  /// Default constructor
  Pimpl (void) { impl_ . reset ( new T ); }

  /// operator <<
  ///   Stream information to graphviz format
  friend std::ostream& 
  operator << ( std::ostream& stream, 
                Pimpl<T> const& t ) { return stream << *t.impl_; }

  /// serialize
  ///   Boost serialization support
  friend class boost::serialization::access;
  template<class Archive> void 
  serialize(Archive & ar, const unsigned int version) { ar & impl_; }
};

#endif
