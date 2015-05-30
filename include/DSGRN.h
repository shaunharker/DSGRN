/// DSGRN.h
/// Shaun Harker
/// 2015-05-29

/// This file provides the Low-Level user API for DSGRN
/// Note: each class declared with the "INTERFACE" macro
///       (which is defined in Idiom.h) comes automatically
///       equipped with a friend function operator << 
///       and support for BOOST serialization and is
///       a shared-pointer-to-implementation wrapper

#ifndef DSGRN_H
#define DSGRN_H

#include "Idiom.h"

INTERFACE(Components) {

  /// iterator 
  typedef std::vector<std::shared_ptr<std::vector<uint64_t>>>::const_iterator iterator;

  /// assign
  ///   Assign data
  void 
  assign ( std::vector<std::shared_ptr<std::vector<uint64_t>>> const& SCCs );

  /// size
  ///   Returns number of vertices
  iterator
  begin ( void ) const;

  /// size
  ///   Returns number of vertices
  iterator
  end ( void ) const;

  /// size
  ///   Returns number of vertices
  uint64_t
  size ( void ) const;

  /// operator []
  ///   Random access to components
  std::shared_ptr<std::vector<uint64_t>>
  operator [] ( uint64_t i ) const;

  /// whichStrong
  ///   Given a vertex determines which 
  ///   strong component it is in
  uint64_t
  whichComponent ( uint64_t i ) const;

};


/// class Digraph
///   This class handles storage of edges between 
///   vertices in the form of adjacency lists.
INTERFACE(Digraph) {

  /// adjacencies (getter)
  ///   Return vector of Vertices which are out-edge adjacencies of input v
  std::vector<uint64_t> const&
  adjacencies ( uint64_t v ) const;

  /// adjacencies (setter)
  ///   Return vector of Vertices which are out-edge adjacencies of input v
  std::vector<uint64_t> &
  adjacencies ( uint64_t v );

  /// size
  ///   Return number of vertices
  uint64_t 
  size ( void ) const;

  /// resize(n)
  ///   Resize so there n vertices.
  ///   Creates empty adjacency lists if needed.
  ///   n must not be smaller than current size
  void
  resize ( uint64_t n );

  /// add_vertex
  ///   Add a vertex, and return the 
  ///   index of the newly added vertex.
  uint64_t
  add_vertex ( void );

  /// add_edge
  ///   Add an edge between source and target
  ///   Warning: does not check if edge already exists
  void
  add_edge ( uint64_t source, uint64_t target );

  /// stringify
  ///   Return a JSON description
  std::string
  stringify ( void ) const;

  /// parse
  ///   Initialize from a JSON description
  void
  parse ( std::string const& str );

};

/// class Poset
INTERFACE(Poset) {

  /// adjacencies (getter)
  ///   Return vector of Vertices which are out-edge adjacencies of input v
  std::vector<uint64_t> const&
  adjacencies ( uint64_t v ) const;

  /// adjacencies (setter)
  ///   Return vector of Vertices which are out-edge adjacencies of input v
  std::vector<uint64_t> &
  adjacencies ( uint64_t v );

  /// size
  ///   Return number of vertices
  uint64_t 
  size ( void ) const;

  /// resize(n)
  ///   Resize so there n vertices.
  ///   Creates empty adjacency lists if needed.
  ///   n must not be smaller than current size
  void
  resize ( uint64_t n );

  /// add_vertex
  ///   Add a vertex, and return the 
  ///   index of the newly added vertex.
  uint64_t
  add_vertex ( void );

  /// add_edge
  ///   Add an edge between source and target
  ///   Warning: does not check if edge already exists
  void
  add_edge ( uint64_t source, uint64_t target );

  /// stringify
  ///   Return a JSON description
  std::string
  stringify ( void ) const;

  /// parse
  ///   Initialize from a JSON description
  void
  parse ( std::string const& str );

  /// reduction
  ///   Perform a transitive reduction
  ///   (i.e. use Hasse diagram representation)
  void
  reduction ( void );

};

/// Annotation
///   A lightweight string container
///   used to annotate Morse Graphs
INTERFACE(Annotation) {

  /// iterator
  ///   STL-style iterator
  typedef std::vector<std::string>::const_iterator iterator;

  /// size
  ///   Return the number of annotations
  uint64_t 
  size ( void ) const;

  /// begin
  ///   Return start iterator
  iterator
  begin ( void ) const;

  /// end
  ///   Return end iterator
  iterator 
  end ( void ) const;

  /// operator []
  ///   Random access to contents
  std::string const&
  operator [] ( uint64_t i ) const;

  /// append
  ///   Add more annotations
  void
  append ( std::string const& label );

  /// stringify
  ///   Return a JSON description
  std::string
  stringify ( void ) const;

  /// parse
  ///   Initialize from a JSON description
  void
  parse ( std::string const& str );

};

/// class MorseDecomposition public interface
INTERFACE(MorseDecomposition) {

  /// assign
  ///   Create a Morse Decomposition given
  ///   a digraph.
  void
  assign ( Digraph const& digraph );

  /// assign
  ///   Create a Morse Decomposition given
  ///   a digraph and its strong components
  ///   (This method is provided in case 
  ///    strong components already computed.)
  void
  assign ( Digraph const& digraph, 
           Components const& components );

  /// poset 
  ///   Return the underlying partial order
  Poset const
  poset ( void ) const;

  /// components 
  ///   Return recurrent components 
  Components const
  components ( void ) const;

};

/// class MorseGraph public interface
INTERFACE(MorseGraph) {
  /// assign (Morse Decomposition)
  ///   Assign data to Morse Graph
  template < class SwitchingGraph > void
  assign ( SwitchingGraph const& sg,
           MorseDecomposition const& md );

  /// poset
  ///   Access poset
  Poset const
  poset ( void ) const;

  /// annotation
  ///   Return the annotation on vertex v.
  Annotation const
  annotation ( uint64_t v ) const;

  /// SHA
  ///   Return a SHA-256 code
  std::string
  SHA256 ( void ) const;

  /// stringify
  ///   Return a JSON description
  std::string
  stringify ( void ) const;

  /// parse
  ///   Initialize from a JSON description
  void
  parse ( std::string const& str );

};

/// Network
///   This class holds network data.
///     * Loads specification files
///     * Outputs Graphviz visualizations
///     * Provides an interface to networks
INTERFACE(Network) {

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

  /// uint64_teraction
  ///   Return the uint64_teraction type of an edge:
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

  /// graphviz
  ///   Return a graphviz string (dot language)
  std::string
  graphviz ( std::vector<std::string> const& theme = 
  { "aliceblue", // background color
    "beige",     // node color
    "black", "darkgoldenrod", "blue", "orange", "red", "yellow" // edge group colors
  } ) const;

};

/// class LogicParameter public interface
INTERFACE(LogicParameter) {

  /// assign
  ///   Assign data
  void
  assign ( uint64_t n, uint64_t m, std::string const& hex );

  /// operator ()
  ///   Return true if input combination drives the output
  ///   to the right, and false if it drives it to the left
  bool
  operator () ( std::vector<bool> const& input_combination, uint64_t output ) const;

  /// operator ()
  ///   Version of operator () where bit has already been computed
  bool
  operator () ( uint64_t bit ) const;

  /// stringify
  ///   Return a JSON-style string
  ///    of form [n,m,"hex"]
  std::string
  stringify ( void ) const;

  /// parse
  ///   Initialize from a JSON-style string
  void
  parse ( std::string const& str );

};

/// class OrderParameter
///   Consider all permutations of m elements,
///   indexed by contiguous integers in lexigraphical
///   order. This class provides methods which 
///   allow one to convert between this indexing
///   and the permutations, along with some convenience 
///   functionality (i.e. applying permutations)
INTERFACE(OrderParameter) {

  /// assign (by index)
  ///   Initialize to the kth permutation of n items
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
  ///   Apply the inverse permuation to input
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

};

/// class Domain
///   This class allows for n-tuples with non-negative
///   entries less than specified limits (set in the
///   the constructor). It provides operator () to access
///   the components as well as increment operators. The
///   increment operator tries to increment the 0th
///   position; if limit is reached it resets to 0 and
///   carried to the next digit, and so forth. Incrementing
///   the largest domain within the limits results in an
///   invalid domain (and isValid stops returning true).
INTERFACE(Domain) {

  /// Domain
  ///   Construct domain object as
  ///   (0,0,...,0) in 
  ///   {0,1,..,limits[0]-1}x...x{0,1,..,limits[D]-1}
  Domain ( std::vector<uint64_t> const& limits );

  /// operator []
  ///   Return dth component
  uint64_t 
  operator [] ( uint64_t d ) const;

  /// operator ++ (preincrement)
  ///   Advance through domain traversal pattern
  Domain & 
  operator ++ ( void );

  /// operator ++ (postincrement)
  ///   Advance through domain traversal pattern
  ///   but return copy of unadvanced domain
  Domain 
  operator ++ ( int );

  /// size
  ///   Return number of dimensions
  uint64_t 
  size ( void ) const;

  /// index
  ///   Return traversal index of domain
  uint64_t 
  index ( void ) const;

  /// setIndex 
  ///   Set the domain by index
  void
  setIndex ( uint64_t i );

  /// left
  ///   Return domain index to the left in dimension d
  ///   (If there is none, behavior is undefined)
  uint64_t
  left ( uint64_t d ) const;

  /// right
  ///   Return domain index to the right in dimension d
  ///   (If there is none, behavior is undefined)
  uint64_t
  right ( uint64_t d ) const;

  /// right
  /// isMin
  ///   Return true if dth component 
  ///   is 0 (i.e. is minimal)
  bool 
  isMin ( uint64_t d ) const;

  /// isMax
  ///   Return true if dth component 
  ///   is limits[d]-1 (i.e. is maximal)
  bool 
  isMax ( uint64_t d ) const;

  /// isValid
  ///   Return true if a valid domain.
  ///   (For use with ++, so one-past-the-end is not valid)
  bool
  isValid ( void ) const;

};

/// class Parameter public interface
INTERFACE(Parameter) {

  /// attracting
  ///   Return true if domain is attracting.
  ///   This is true iff none of the walls of
  ///   are absorbing.
  bool 
  attracting ( Domain const& dom ) const;

  /// absorbing
  ///   Return true if wall is absorbing
  ///   The wall is described by a domain dom,
  ///   a dimension collapse_dim, and a direction. 
  ///   The direction is either -1 or +1, signifying
  ///   the left wall or the right wall with 
  ///   collapse dimension collapse_dim.
  bool
  absorbing ( Domain const& dom, int collapse_dim, int direction ) const;

  /// assign
  ///   Assign data to parameter
  void
  assign ( std::vector<LogicParameter> const& logic,
           std::vector<OrderParameter> const& order, 
           Network const& network );

  /// assign (network-only)
  ///   Assign network to parameter.
  ///   logic and order left uninitialized.
  void
  assign ( Network const& network );

  /// network
  ///   Return network
  Network const
  network ( void ) const;

  /// stringify
  ///   Return a JSON description
  std::string
  stringify ( void ) const;

  /// parse
  ///   Initialize from a JSON description
  void
  parse ( std::string const& str );

  /// inequalities
  ///    Output a list of inequalities correspond to the parameter
  std::string
  inequalities ( void ) const;

};

/// class ParameterGraph public interface
INTERFACE(ParameterGraph) {

  /// assign
  ///   Assign a network to the parameter graph
  ///   Search in path for logic .dat files
  void
  assign ( Network const& network, 
           std::string const& path = std::string("./data/logic/") );

  /// size
  ///   Return the number of parameters
  uint64_t
  size ( void ) const;

  /// parameter
  ///   Return the parameter associated with an index
  Parameter
  parameter ( uint64_t index ) const;

  /// index
  ///   Return the index associated with a parameter
  uint64_t
  index ( Parameter const& p ) const;

  /// adjacencies
  ///   Return the adjacent parameters to a given parameter
  std::vector<uint64_t>
  adjacencies ( uint64_t index ) const;

  /// network
  ///   Return network
  Network const
  network ( void ) const;

  /// fixedordersize
  ///   Return the number of parameters
  ///   for a fixed ordering
  uint64_t
  fixedordersize ( void ) const;

  /// reorderings
  ///   Return of reorderings
  ///   Note: size() = fixedordersize()*reorderings()
  uint64_t
  reorderings ( void ) const;

};

INTERFACE(DomainGraph) {

  /// assign
  ///   Construct based on parameter and network
  void
  assign ( Parameter const& parameter );

  /// digraph
  ///   Return underlying digraph
  Digraph const
  digraph ( void ) const;

  /// annotate
  Annotation const
  annotate ( std::vector<uint64_t> const& vertices ) const;

};

INTERFACE(Wall) {

  /// Wall 
  ///   Default constructor
  Wall ( void );

  /// Wall
  ///   Construct from domain collapse (see assign)
  Wall ( Domain const& dom, uint64_t collapse_dim, int direction );

  /// assign 
  ///   Create wall from domain, dimension of collapse, 
  ///   and direction of collapse.
  void
  assign ( Domain const& dom, uint64_t collapse_dim, int direction );

  /// index
  ///   Return a unique index. Note wall indices are 
  ///   not guaranteed to be contiguous.
  uint64_t
  index ( void ) const;

};

INTERFACE(WallGraph) {

  /// assign
  ///   Construct based on parameter and network
  void
  assign ( Parameter const& parameter );

  /// digraph
  ///   Return underlying digraph
  Digraph const
  digraph ( void ) const;

  /// annotate
  Annotation const
  annotate ( std::vector<uint64_t> const& vertices ) const;

};

#endif
