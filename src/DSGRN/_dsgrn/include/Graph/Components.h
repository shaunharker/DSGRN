/// Components.h
/// Shaun Harker
/// 2015-05-24

#pragma once

#include "common.h"

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

// /// Python Bindings

// #include <pybind11/pybind11.h>
// #include <pybind11/stl.h>
// namespace py = pybind11;

// inline void
// ComponentsBinding (py::module &m) {
//   py::class_<Components, std::shared_ptr<Components>>(m, "Components")
//     .def(py::init<>())
//     .def(py::init<std::vector<uint64_t>const&,std::vector<bool>const&,std::vector<bool>const&>())
//     .def("begin", &Components::begin)
//     .def("end", &Components::end)
//     .def("size", &Components::size)
//     .def("__iter__", [](Components const& v) {
//         return py::make_iterator(v.begin(), v.end());
//       }, py::keep_alive<0, 1>())        
//     .def("recurrentComponents", &Components::recurrentComponents)
//     .def("isRecurrent", &Components::isRecurrent)
//     .def("whichComponent", &Components::whichComponent);
// }
