/// OrderParameter.h
/// Shaun Harker
/// 2015-05-24

#pragma once

#include "common.h"

struct OrderParameter_;

/// class OrderParameter
///   Consider all permutations of m elements,
///   indexed by contiguous integers in lexigraphical
///   order. This class provides methods which
///   allow one to convert between this indexing
///   and the permutations, along with some convenience
///   functionality (i.e. applying permutations)
class OrderParameter {
public:
  /// OrderParameter
  OrderParameter ( void );

  /// OrderParameter (by index)
  ///   Initialize to the kth permutation of m items
  ///   The permutations are ordered lexicographically
  OrderParameter ( uint64_t m, uint64_t k );

  /// OrderParameter (by permutation)
  ///   Initialize as given permutation
  OrderParameter ( std::vector<uint64_t> const& perm );

  /// assign (by index)
  ///   Initialize to the kth permutation of m items
  ///   The permutations are ordered lexicographically
  void
  assign ( uint64_t m, uint64_t k );

  /// assign (by permutation)
  ///   Initialize as given permutation
  void
  assign ( std::vector<uint64_t> const& perm );

  /// operator ()
  ///   Apply the permutation to the input
  uint64_t
  operator () ( uint64_t i ) const;

  /// inverse
  ///   Apply the inverse permutation to input
  uint64_t
  inverse ( uint64_t i ) const;

  /// permutation ()
  ///   Return the underlying permutation
  std::vector<uint64_t> const&
  permutation ( void ) const;

  /// index ()
  ///   Return the index of the permutation
  uint64_t
  index ( void ) const;

  /// size ()
  ///   Return the number of items
  uint64_t
  size ( void ) const;

  /// stringify
  ///   Return a JSON-style string
  ///    of form [p1, p2, p3, ...]
  std::string
  stringify ( void ) const;

  /// parse
  ///   Initialize from a JSON-style string
  void
  parse ( std::string const& str );

  /// adjacencies ()
  ///   Return the adjacent OrderParameters
  std::vector<OrderParameter>
  adjacencies ( void ) const;

  /// operator ==
  ///   Equality comparison
  bool
  operator == ( OrderParameter const& rhs ) const;
  
  /// operator <<
  ///   Output debug data to stream
  friend std::ostream& operator << ( std::ostream& stream, OrderParameter const& p );

private:
  std::shared_ptr<OrderParameter_> data_;
  static std::vector<uint64_t> _index_to_tail_rep ( uint64_t index );
  static std::vector<uint64_t> _tail_rep_to_perm ( std::vector<uint64_t> const& tail_rep );
  static std::vector<uint64_t> _perm_to_tail_rep ( std::vector<uint64_t> const& perm );
  static uint64_t _tail_rep_to_index ( std::vector<uint64_t> const& tail_rep );
};

struct OrderParameter_ {
  std::vector<uint64_t> permute_;
  std::vector<uint64_t> inverse_;
  uint64_t k_;
  uint64_t m_;
};

/// Python Bindings

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
namespace py = pybind11;

inline void
OrderParameterBinding (py::module &m) {
  py::class_<OrderParameter, std::shared_ptr<OrderParameter>>(m, "OrderParameter")
    .def(py::init<>())
    .def(py::init<uint64_t, uint64_t>())
    .def(py::init<std::vector<uint64_t> const&>())
    .def("__call__", &OrderParameter::operator())
    .def("inverse", &OrderParameter::inverse)
    .def("permutation", &OrderParameter::permutation)
    .def("index", &OrderParameter::index)
    .def("size", &OrderParameter::size)
    .def("stringify", &OrderParameter::stringify)
    .def("parse", &OrderParameter::parse)
    .def("adjacencies", &OrderParameter::adjacencies)
    .def("__eq__", &OrderParameter::operator==)
    .def("__str__", [](OrderParameter * lp){ std::stringstream ss; ss << *lp; return ss.str();; })
    .def(py::pickle(
    [](OrderParameter const& p) { // __getstate__
        /* Return a tuple that fully encodes the state of the object */
        return py::make_tuple(p.size(), p.index());
    },
    [](py::tuple t) { // __setstate__
        if (t.size() != 2)
            throw std::runtime_error("Unpickling OrderParameter object: Invalid state!");
        /* Create a new C++ instance */
        return OrderParameter(t[0].cast<uint64_t>(), t[1].cast<uint64_t>());
    }));
}
