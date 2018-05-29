/// NFA.hpp
/// Shaun Harker
/// 2018-05-17
/// MIT LICENSE

#include "NFA.h"

inline NFA::NFA ( void ) {}

inline void NFA::
set_initial ( uint64_t idx ) { initial_idx_ = idx; }

inline void NFA::
set_final ( uint64_t idx ) { final_idx_ = idx; }

inline uint64_t NFA::
initial ( void ) const { return initial_idx_; }

inline uint64_t NFA::
final ( void ) const { return final_idx_; }

inline std::pair<NFA, std::vector<std::pair<uint64_t,uint64_t>>> NFA::
intersect ( NFA const& lhs, NFA const& rhs ) {
  NFA nfa;
  typedef std::pair<uint64_t,uint64_t> Node;
  std::unordered_map<Node, uint64_t, dsgrn::hash<Node>> nodes;
  std::vector<Node> node_by_index;
  std::stack<std::pair<uint64_t,uint64_t>> work_stack;
  Node start {lhs.initial(),rhs.initial()};
  Node stop {lhs.final(),rhs.final()};
  auto vertex_index = [&](Node const& n) {
    if ( nodes.count(n) == 0 ) {
      work_stack.push(n);
      nodes[n] = nfa.add_vertex();
      node_by_index.push_back(n);
    }
    return nodes[n];
  };
  nfa.set_initial(vertex_index(start));
  nfa.set_final(vertex_index(stop));
  while ( not work_stack.empty() ) {
    // Each iteration, we process a pair (a,b) where a is a node in lhs and b is a node in rhs
    // We follow all edges from a and all edges from b with the same label.
    // New vertices are assigned indices as they are discovered (upon discovery they are also placed on a stack
    // to be processed as well). 
    auto node = work_stack.top();
    work_stack.pop();
    auto i = nodes[node];
    auto L_adj = lhs.adjacencies(node.first); // type std::map<LabelledMultidigraph::LabelType, std::unordered_set<uint64_t>>
    auto R_adj = rhs.adjacencies(node.second);

    for ( auto const& label_children : L_adj) {
      auto label = label_children.first;
      if ( not R_adj.count(label) ) continue;
      auto const& Lchildren = label_children.second;
      auto const& Rchildren = R_adj.find(label) -> second;
      for ( auto const& lchild : Lchildren ) {
        for ( auto const& rchild : Rchildren ) {
          auto j = vertex_index({lchild, rchild});
          nfa.add_edge(i,j,label);
        }
      }
    }
  }
  return {nfa,node_by_index};
}

inline uint64_t NFA::
count_paths ( void ) const {
  return -1;
}

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
      for ( auto j : set ) { 
        if ( label == ' ' && i == j) continue;
        ss << i << " -> " << j << "[label=\"" << label << "\"]\n";
      }
    }
  }
  ss << "}\n";
  return ss.str();
}
