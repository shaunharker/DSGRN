/// Components.h
/// Shaun Harker
/// 2015-05-24

#pragma once

#include "common.h"

template < typename I >
class Range {
public:
  typedef I iterator;
  typedef I const_iterator;
  Range ( void ) {}
  Range ( iterator b, iterator e ) : begin_(b), end_(e) {}
  iterator begin ( void ) const { return begin_;}
  iterator end ( void ) const { return end_;}
  uint64_t size ( void ) const { return end_ - begin_;}
  typename iterator::value_type operator [] ( int64_t i ) const {return *(begin_ + i);}
private:
  iterator begin_;
  iterator end_;
};

/// fun_iterator
/// like a boost::counting_iterator except it dereferences to f(i) instead of i
///   Note: f(i) has type T, which is the template parameter
template < typename T >
class fun_iterator {
public:
    typedef fun_iterator self_type;
    typedef T value_type;
    typedef T& reference;
    typedef T* pointer;
    typedef int64_t difference_type;
    typedef std::forward_iterator_tag iterator_category;
    fun_iterator(void){}
    fun_iterator(int64_t i, std::function<T(int64_t)> f) : f_(f), val_(i) { }
    fun_iterator(std::function<T(int64_t)> f) : f_(f), val_(0) { }
    self_type operator++() { val_++; return *this; }
    self_type operator++(int) { self_type i = *this; val_++; return i; }
    self_type operator+(int64_t i) const {return fun_iterator(val_ + i, f_);}
    difference_type operator-(self_type const& rhs) const{return val_ - rhs.val_;}
    T operator*() const { return f_(val_); }
    //const T* operator->() { return ptr_; }
    self_type operator=(const self_type& other) { val_ = other.val_; f_ = other.f_; return *this; }
    bool operator==(const self_type& rhs)const { return val_ == rhs.val_; }
    bool operator!=(const self_type& rhs)const { return val_ != rhs.val_; }
private:
    std::function<T(int64_t)> f_;
    int64_t val_;
};

typedef Range<std::vector<uint64_t>::const_iterator> Component;
typedef fun_iterator<Component> ComponentIterator;

struct Components_;


class Components {
public:
  typedef std::function < Component ( int64_t ) > Functor;
  typedef fun_iterator<Component> iterator;
  typedef Range<iterator> ComponentContainer;

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
  
  /// ptr
  uint64_t
  ptr ( void ) const {
    return (uint64_t) data_.get();
  }

  /// operator <<
  ///   Output to stream
  friend std::ostream& operator << ( std::ostream& stream, Components const& c );

private: 
  std::shared_ptr<Components_> data_;
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
};

/// Python Bindings

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
namespace py = pybind11;

inline void
ComponentsBinding (py::module &m) {
  py::class_<Component, std::shared_ptr<Component>>(m, "Component")
    .def(py::init<>())
    .def(py::init<std::vector<uint64_t>::const_iterator,std::vector<uint64_t>::const_iterator>())
    .def("begin", &Component::begin)
    .def("end", &Component::end)
    .def("size", &Component::size)
    .def("__iter__", [](Component const& v) {
        return py::make_iterator(v.begin(), v.end());
      }, py::keep_alive<0, 1>());

  py::class_<fun_iterator<Component>, std::shared_ptr<fun_iterator<Component>>>(m, "ComponentIterator");

  py::class_<Components::ComponentContainer, std::shared_ptr<Components::ComponentContainer>>(m, "ComponentContainer")
    .def(py::init<>())
    .def("begin", &Components::ComponentContainer::begin)
    .def("end", &Components::ComponentContainer::end)
    .def("size", &Components::ComponentContainer::size)
    .def("__iter__", [](Components::ComponentContainer const& v) {
        return py::make_iterator(v.begin(), v.end());
      }, py::keep_alive<0, 1>());

  py::class_<Components, std::shared_ptr<Components>>(m, "Components")
    .def(py::init<>())
    .def(py::init<std::vector<uint64_t>const&,std::vector<bool>const&,std::vector<bool>const&>())
    .def("begin", &Components::begin)
    .def("end", &Components::end)
    .def("size", &Components::size)
    .def("__iter__", [](Components const& v) {
        return py::make_iterator(v.begin(), v.end());
      }, py::keep_alive<0, 1>())        
    .def("recurrentComponents", &Components::recurrentComponents)
    .def("isRecurrent", &Components::isRecurrent)
    .def("whichComponent", &Components::whichComponent)
    .def("ptr", &Components::ptr)
    .def("__str__", [](Components * c){ std::stringstream ss; ss << *c; return ss.str(); });
}
