/// SearchGraph.h
/// Shaun Harker and Bree Cummins
/// 2016-03-19

#ifndef SEARCHGRAPH_H
#define SEARCHGRAPH_H

struct SearchGraph_;

class SearchGraph {
public:
  /// SearchGraph
  ///   Default constructor
  SearchGraph ( void );

  /// SearchGraph
  ///   Create search graph from a domain graph and a morse set index
  SearchGraph ( DomainGraph const& dg, uint64_t morse_set_index );

  /// assign
  ///   Create search graph from a domain graph and a morse set index
  void
  assign ( DomainGraph const& dg, uint64_t morse_set_index );

  /// size
  ///   Return the number of vertices in the search graph
  uint64_t
  size ( void ) const;

  /// label
  ///   Given a vertex, return the associated label
  std::vector<uint8_t>
  label ( uint64_t v ) const;

  /// adjacencies
  ///   Given a vertex v, return vector of vertices which 
  ///   are out-edge adjacencies of input v
  std::vector<uint64_t> const&
  adjacencies ( uint64_t v ) const;

  /// switching
  ///   Given a source and target domain give the switching 
  ///   variable associated with the wall between them
  uint64_t 
  switching ( uint64_t source, uint64_t target ) const;

private:
  std::shared_ptr<SearchGraph_> data_;
  /// serialize
  ///   For use with BOOST Serialization library,
  ///   which is used by the cluster-delegator MPI package
  friend class boost::serialization::access;
  template<class Archive>
  void serialize(Archive & ar, const unsigned int version) {
    ar & data_;
  }
};

struct SearchGraph_ {
  
  // TODO: determine data required.

  /// serialize
  ///   For use with BOOST Serialization library,
  ///   which is used by the cluster-delegator MPI package
  friend class boost::serialization::access;
  template<class Archive>
  void serialize(Archive & ar, const unsigned int version) {
    ar & adjacencies_;
  }
};

#endif
