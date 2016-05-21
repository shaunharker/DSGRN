
#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>

#include "DSGRN.h"

#include <vector>

/// Wrapping code for Network.h

// IntList 
typedef std::vector<uint64_t> IntList;
std::string
printIntList ( IntList const* self ) {
  std::stringstream ss;
  bool first = true;
  ss << "[";
  for ( auto const& item : *self ) {
    if ( first ) first = false; else ss << ", ";
    ss << item;
  }
  ss << "]";
  return ss.str();
}

// IntListList
typedef std::vector<std::vector<uint64_t>> IntListList;
std::string
printIntListList ( IntListList const* self ) {
  std::stringstream ss;
  bool first = true;
  ss << "[";
  for ( auto const& item : *self ) {
    if ( first ) first = false; else ss << ", ";
    ss << printIntList(&item);
  }
  ss << "]";
  return ss.str();
}

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(network_overloads, graphviz, 0, 1)


BOOST_PYTHON_MODULE(libpyDSGRN)
{
    using namespace boost::python;

    // Create the Python type object for our extension class and define __init__ function.

    class_<IntList>("IntList")
      .def(vector_indexing_suite<IntList>() )
      .def("__str__", &printIntList)
      .def("__repr__", &printIntList)
    ;

    class_<IntListList>("IntListList")
      .def(vector_indexing_suite<IntListList>() )
      .def("__str__", &printIntListList)
      .def("__repr__", &printIntListList)
    ;

    class_<Network>("Network", init<std::string>())
      .def(init<>())
      .def("load", &Network::load)  
      .def("assign", &Network::assign)  
      .def("size", &Network::size)  
      .def("index", &Network::index)  
      .def("name", &Network::name, return_value_policy<copy_const_reference>()) 
      .def("inputs", &Network::inputs, return_value_policy<copy_const_reference>()) 
      .def("outputs", &Network::inputs, return_value_policy<copy_const_reference>()) 
      .def("logic", &Network::logic, return_value_policy<copy_const_reference>())
      .def("essential", &Network::essential) 
      .def("interaction", &Network::interaction) 
      .def("order", &Network::order) 
      .def("domains", &Network::domains) 
      .def("specification", &Network::specification)
      .def("graphviz", &Network::graphviz, network_overloads()) 
      .def("__str__", &Network::specification)
    ;
}
