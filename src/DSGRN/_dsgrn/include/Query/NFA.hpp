/// NFA.hpp
/// Shaun Harker
/// 2018-02-20
/// MIT LICENSE

#include "NFA.h"

inline std::string 
NFA::graphviz ( void ) const {
  using namespace NFA_detail;
  std::stringstream ss;
  ss << "digraph G {\n";
  for ( uint64_t i = 0; i < num_vertices(); ++ i ) {
    ss << i << " [label=\"" << i 
       << ((i == initial()) ? "(initial)" : "") << ((i == final()) ? "(final)" : "") << "\"];\n";
  }

  // Queue the remaining children.
  for ( uint64_t i = 0; i < num_vertices(); ++ i ) {
    for ( auto label_set : adjacencies(i) ) {
      auto label = label_set.first;
      auto set = label_set.second;
      for ( auto j : set ) ss << i << " -> " << j << "[label=\"" << label << "\"]\n";
    }
  }
  ss << "}\n";
  return ss.str();
}
