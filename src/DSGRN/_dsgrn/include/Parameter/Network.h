/// Network.h
/// Shaun Harker
/// 2015-05-22

#pragma once

#include "common.h"

class Network_;

/// Network
///   This class holds network data.
///     * Loads specification files
///     * Outputs Graphviz visualizations
///     * Provides an interface to networks
class Network {
public:
  /// constructor
  Network ( void );

  /// Network
  ///   Construct network
  ///   If s contains a colon character (i.e. ':') it assumes s is a network specification.
  ///   Otherwise, it assumes s is a filename and attempts to load a network specification.
  Network ( std::string const& s );

  /// assign
  ///   Delayed construction of default constructed object
  void
  assign ( std::string const& s );
  
  /// load
  ///   load from network specification file
  void 
  load ( std::string const& filename );

  /// size
  ///   Return the number of nodes in the network
  uint64_t
  size ( void ) const;

  /// index
  ///   Return index of node given name string
  uint64_t 
  index ( std::string const& name ) const;

  /// name
  ///   Return name of node (given by index)
  std::string const&
  name ( uint64_t index ) const;

  /// inputs
  ///   Return a list of inputs to a node
  std::vector<uint64_t> const&
  inputs ( uint64_t index ) const;

  /// outputs
  ///   Return a list of outputs to a node
  std::vector<uint64_t> const&
  outputs ( uint64_t index ) const;

  /// logic
  ///   Return the logic of a node (given by index)
  std::vector<std::vector<uint64_t>> const&
  logic ( uint64_t index ) const;

  /// essential
  ///   Return whether or not to use only essential logic parameters
  bool
  essential ( uint64_t index ) const;

  /// interaction
  ///   Return the interaction type of an edge:
  ///   False for repression, true for activation
  bool
  interaction ( uint64_t source, uint64_t target ) const;

  /// order
  ///   Return the out-edge order number of an edge, i.e. so
  ///   outputs(source)[order(source,target)] == target
  uint64_t
  order ( uint64_t source, uint64_t target ) const;

  /// domains
  ///   Return a list consisting of the number of 
  ///   domains across (i.e. number of out-edges plus one)
  ///   for each dimension (i.e. network node)
  std::vector<uint64_t>
  domains ( void ) const;

  /// specification
  ///    Return the specification string (i.e. network spec file)
  std::string
  specification ( void ) const;

  /// graphviz
  ///   Return a graphviz string (dot language)
  std::string
  graphviz ( std::vector<std::string> const& theme = 
  { "aliceblue", // background color
    "beige",     // node color
    "black", "darkgoldenrod", "blue", "orange", "red", "yellow" // edge group colors
  } ) const;

  /// operator <<
  friend std::ostream& operator << ( std::ostream& stream, Network const& network );

private:
  std::shared_ptr<Network_> data_;

  std::vector<std::string> _lines ( void );
  void _parse ( std::vector<std::string> const& lines );
};

struct Network_ {
  std::vector<std::vector<uint64_t>> inputs_;
  std::vector<std::vector<uint64_t>> outputs_;
  std::unordered_map<std::string, uint64_t> index_by_name_;
  std::vector<std::string> name_by_index_;
  std::unordered_map<std::pair<uint64_t,uint64_t>, bool, dsgrn::hash<std::pair<uint64_t,uint64_t>>> edge_type_;
  std::unordered_map<std::pair<uint64_t,uint64_t>, uint64_t, dsgrn::hash<std::pair<uint64_t,uint64_t>>> order_;
  std::vector<std::vector<std::vector<uint64_t>>> logic_by_index_;
  std::vector<bool> essential_;
  std::string specification_;
};

/// Python Bindings

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
namespace py = pybind11;

inline void
NetworkBinding (py::module &m) {
  py::class_<Network, std::shared_ptr<Network>>(m, "Network")
    .def(py::init<>())
    .def(py::init<std::string const&>())
    .def("load", &Network::load)
    .def("assign", &Network::assign)
    .def("size", &Network::size)
    .def("index", &Network::index)
    .def("name", &Network::name)
    .def("inputs", &Network::inputs)
    .def("outputs", &Network::outputs)
    .def("logic", &Network::logic)
    .def("essential", &Network::essential)
    .def("interaction", &Network::interaction)
    .def("order", &Network::order)
    .def("domains", &Network::domains)
    .def("specification", &Network::specification)
    .def("graphviz", [](Network const& network){ return network.graphviz();})
    .def(py::pickle(
    [](Network const& p) { // __getstate__
        /* Return a tuple that fully encodes the state of the object */
        return py::make_tuple(p.specification());
    },
    [](py::tuple t) { // __setstate__
        if (t.size() != 1)
            throw std::runtime_error("Unpickling Network object: Invalid state!");
        /* Create a new C++ instance */
        return Network(t[0].cast<std::string>());
    }));
}
