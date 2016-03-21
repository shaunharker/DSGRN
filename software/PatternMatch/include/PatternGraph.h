/// PatternGraph.h
/// Shaun Harker and Bree Cummins
/// 2016-03-19

#ifndef PATTERNGRAPH_H
#define PATTERNGRAPH_H

struct PatternGraph_;

class PatternGraph {
public:
  /// PatternGraph
  ///   Default constructor
  PatternGraph ( void );

  /// PatternGraph
  ///   Create a pattern graph from a poset
  PatternGraph ( Poset const& poset );

  /// assign
  ///   Create a pattern graph from a poset
  void
  assign ( Poset const& poset );

  /// root
  ///   Return the index of the root vertex
  uint64_t 
  root ( void ) const;

  /// leaf
  ///   Return the index of the leaf vertex
  uint64_t 
  leaf ( void ) const;

  /// size
  ///   Return the number of vertices in the pattern graph
  uint64_t
  size ( void ) const;

  /// label
  ///   Given a vertex, return the associated label
  std::vector<uint8_t>
  label ( uint64_t v ) const;
  
  /// consume
  ///   Given a vertex v and a variable, report the unique vertex u, if it exists,
  ///   in the adjacency list of vertex. This corresponds to "consuming" the edge
  ///   in the pattern graph.
  uint64_t 
  consume ( uint64_t vertex, uint64_t variable ) const;

private:
  std::shared_ptr<PatternGraph_> data_;
  /// serialize
  ///   For use with BOOST Serialization library,
  ///   which is used by the cluster-delegator MPI package
  friend class boost::serialization::access;
  template<class Archive>
  void serialize(Archive & ar, const unsigned int version) {
    ar & data_;
  }
};

struct PatternGraph_ {
  uint64_t root_;
  uint64_t leaf_;
  uint64_t size_;
  std::vector<std::vector<uint8_t>> labels_;
  std::vector<std::unordered_map<uint64_t, uint64_t>> consume_;
  /// serialize
  ///   For use with BOOST Serialization library,
  ///   which is used by the cluster-delegator MPI package
  friend class boost::serialization::access;
  template<class Archive>
  void serialize(Archive & ar, const unsigned int version) {
    ar & root_;
    ar & leaf_;
    ar & size_;
    ar & labels_;
    ar & consume_;
  }
};

#endif
