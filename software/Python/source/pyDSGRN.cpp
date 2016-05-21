
#include <boost/python.hpp>
#include "DSGRN.h"

#include <vector>

char const* yay()
{
   return "hello, world";
}

std::vector<uint64_t>
vec ( void ) {
  return std::vector<uint64_t> ( 5, 5 );
}


BOOST_PYTHON_MODULE(libpyDSGRN)
{
    using namespace boost::python;
    def("yay", yay);
    def("vec", vec);

    // Create the Python type object for our extension class and define __init__ function.
    class_<Network>("Network", init<std::string>())
        .def("size", &Network::size)  
        //.def("index", &Network::index)  
        //.def("name", &Network::name)  
    ;
}
