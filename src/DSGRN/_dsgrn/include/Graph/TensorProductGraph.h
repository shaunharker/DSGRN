/// TensorProductGraph.h
/// Shaun Harker
/// 2018-02-20
/// MIT LICENSE

#include "LabelledMultidigraph.h"

class TensorProductGraph : public LabelledMultidigraph {
public:

  /// TensorProductGraph
  TensorProductGraph ( LabelledMultidigraph const& lhs, LabelledMultidigraph const& rhs );

  /// graphviz (override)
  std::string 
  graphviz ( void ) const;

  /// p
  ///  return coordinates of vertex
  std::pair<uint64_t, uint64_t>
  p ( uint64_t v ) const;

  /// q
  ///   Given coordinates, return index of node
  ///   p(q)
  uint64_t
  q ( std::pair<uint64_t, uint64_t> v ) const;

private:
  ///
  std::unordered_map<LabelType, std::unordered_set<std::pair<uint64_t, uint64_t>>>
  tp_adjacencies ( uint64_t i, uint64_t j ) const;

  std::unordered_map<std::pair<uint64_t, uint64_t>, uint64_t> q_;
  std::vector<std::pair<uint64_t, uint64_t>> p_;
}

