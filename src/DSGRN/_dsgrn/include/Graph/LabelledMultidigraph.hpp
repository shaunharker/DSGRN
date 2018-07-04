/// LabelledMultidigraph.hpp
/// Shaun Harker
/// 2018-02-22
/// MIT LICENSE

#include "LabelledMultidigraph.h"

inline
LabelledMultidigraph::LabelledMultidigraph ( void ) {}

inline uint64_t 
LabelledMultidigraph::add_vertex ( void ) {
  adj_ . push_back ( std::map<LabelType, std::unordered_set<uint64_t>>());
  return adj_.size() - 1;
}

inline void 
LabelledMultidigraph::add_edge ( uint64_t i, uint64_t j, LabelledMultidigraph::LabelType l ) {
  adj_[i][l].insert(j);
} 

inline uint64_t
LabelledMultidigraph::num_vertices ( void ) const {
  return adj_.size();
}

inline std::map<LabelledMultidigraph::LabelType, std::unordered_set<uint64_t>> const&
LabelledMultidigraph::adjacencies ( uint64_t v ) const {
  if ( v >= adj_.size() ) throw std::invalid_argument("LabelledMultidigraph::adjacencies: vertex out of range");
  return adj_[v];
}

inline std::unordered_set<uint64_t> LabelledMultidigraph::
unlabelled_adjacencies( uint64_t v ) const {
  auto adj = adjacencies(v);
  std::unordered_set<uint64_t> result;
  for ( auto x : adj ) result.insert(x.second.begin(),x.second.end());
  return result;
}

inline std::string 
LabelledMultidigraph::graphviz ( void ) const {
  std::stringstream ss;
  ss << "digraph G {\n";
  for ( uint64_t i = 0; i < num_vertices(); ++ i ) {
    ss << i << " [label=\"" << i << "\"];\n";
  }
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
