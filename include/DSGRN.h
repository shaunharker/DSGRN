/// DSGRN.h
/// Shaun Harker
/// 2015-05-29

/// This file provides the Low-Level user API for DSGRN
/// Note: each class declared with the "INTERFACE" macro
///       (which is defined in Idiom.h) comes automatically
///       equipped with a friend function operator << 
///       and support for BOOST serialization

#ifndef DSGRN_H
#define DSGRN_H

// Project header files
#include "Database/json.h"
#include "Database/SHA256.h"
#include "Database/sqlambda.h"
#include "Idiom.h"

class Network; class LogicParameter; class OrderParameter; class Parameter; 
class ParameterGraph; class DomainGraph; class Domain; class WallGraph; 
class Wall; class Digraph; class Poset; class Components; class Annotation; 
class MorseDecomposition; class MorseGraph;

// Interfaces

#include "Graph/Components.h"

INTERFACE(Components) {

  /// iterator 
  typedef std::vector<std::shared_ptr<std::vector<uint64_t>>>::const_iterator iterator;

  /// assign
  ///   Assign data
  void
  assign ( std::vector<std::shared_ptr<std::vector<uint64_t>>> const& SCCs )
          IMPLEMENT(assign(SCCs));

  /// size
  ///   Returns number of vertices
  iterator
  begin ( void ) const
          IMPLEMENT(begin());

  /// size
  ///   Returns number of vertices
  iterator
  end ( void ) const
          IMPLEMENT(end());

  /// size
  ///   Returns number of vertices
  uint64_t
  size ( void ) const
          IMPLEMENT(size());

  /// operator []
  ///   Random access to components
  std::shared_ptr<std::vector<uint64_t>>
  operator [] ( uint64_t i ) const
          IMPLEMENT(operator[](i));

  /// whichStrong
  ///   Given a vertex determines which 
  ///   strong component it is in
  uint64_t
  whichComponent ( uint64_t i ) const
          IMPLEMENT(whichComponent(i));

};

#include "Graph/Digraph.h"

/// class Digraph
///   This class handles storage of edges between 
///   vertices in the form of adjacency lists.
INTERFACE(Digraph) {

  /// adjacencies (getter)
  ///   Return vector of Vertices which are out-edge adjacencies of input v
  std::vector<uint64_t> const&
  adjacencies ( uint64_t v ) const
          IMPLEMENT(adjacencies(v));

  /// adjacencies (setter)
  ///   Return vector of Vertices which are out-edge adjacencies of input v
  std::vector<uint64_t> &
  adjacencies ( uint64_t v )
          IMPLEMENT(adjacencies(v));

  /// size
  ///   Return number of vertices
  uint64_t 
  size ( void ) const
          IMPLEMENT(size());

  /// resize(n)
  ///   Resize so there n vertices.
  ///   Creates empty adjacency lists if needed.
  ///   n must not be smaller than current size
  void
  resize ( uint64_t n )
          IMPLEMENT(resize(n));

  /// add_vertex
  ///   Add a vertex, and return the 
  ///   index of the newly added vertex.
  uint64_t
  add_vertex ( void )
          IMPLEMENT(add_vertex());

  /// add_edge
  ///   Add an edge between source and target
  ///   Warning: does not check if edge already exists
  void
  add_edge ( uint64_t source, uint64_t target )
          IMPLEMENT(add_edge(source,target));

  /// stringify
  ///   Return a JSON description
  std::string
  stringify ( void ) const
          IMPLEMENT(stringify());

  /// parse
  ///   Initialize from a JSON description
  void
  parse ( std::string const& str )
          IMPLEMENT(parse(str));

};

#include "Graph/Poset.h"

/// class Poset
INTERFACE(Poset) {

  /// adjacencies (getter)
  ///   Return vector of Vertices which are out-edge adjacencies of input v
  std::vector<uint64_t> const&
  adjacencies ( uint64_t v ) const
          IMPLEMENT(adjacencies(v));

  /// adjacencies (setter)
  ///   Return vector of Vertices which are out-edge adjacencies of input v
  std::vector<uint64_t> &
  adjacencies ( uint64_t v )
          IMPLEMENT(adjacencies(v));

  /// size
  ///   Return number of vertices
  uint64_t 
  size ( void ) const
          IMPLEMENT(size());

  /// resize(n)
  ///   Resize so there n vertices.
  ///   Creates empty adjacency lists if needed.
  ///   n must not be smaller than current size
  void
  resize ( uint64_t n )
          IMPLEMENT(resize(n));

  /// add_vertex
  ///   Add a vertex, and return the 
  ///   index of the newly added vertex.
  uint64_t
  add_vertex ( void )
          IMPLEMENT(add_vertex());

  /// add_edge
  ///   Add an edge between source and target
  ///   Warning: does not check if edge already exists
  void
  add_edge ( uint64_t source, uint64_t target )
          IMPLEMENT(add_edge(source,target));

  /// stringify
  ///   Return a JSON description
  std::string
  stringify ( void ) const
          IMPLEMENT(stringify());

  /// parse
  ///   Initialize from a JSON description
  void
  parse ( std::string const& str )
          IMPLEMENT(parse(str));

  /// reduction
  ///   Perform a transitive reduction
  ///   (i.e. use Hasse diagram representation)
  void
  reduction ( void )
          IMPLEMENT(reduction());

};

#include "Graph/StrongComponents.h"

#include "Dynamics/Annotation.h"

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
  size ( void ) const
          IMPLEMENT(size());

  /// begin
  ///   Return start iterator
  iterator
  begin ( void ) const
          IMPLEMENT(begin());

  /// end
  ///   Return end iterator
  iterator 
  end ( void ) const
          IMPLEMENT(end());

  /// operator []
  ///   Random access to contents
  std::string const&
  operator [] ( uint64_t i ) const
          IMPLEMENT(operator[](i));

  /// append
  ///   Add more annotations
  void
  append ( std::string const& label )
          IMPLEMENT(append(label));

  /// stringify
  ///   Return a JSON description
  std::string
  stringify ( void ) const
          IMPLEMENT(stringify());

  /// parse
  ///   Initialize from a JSON description
  void
  parse ( std::string const& str )
          IMPLEMENT(parse(str));

};

#include "Dynamics/MorseDecomposition.h"

INTERFACE(MorseDecomposition) {

  /// assign
  ///   Create a Morse Decomposition given
  ///   a digraph.
  void
  assign ( Digraph const& digraph )
          IMPLEMENT(assign(digraph));

  /// assign
  ///   Create a Morse Decomposition given
  ///   a digraph and its strong components
  ///   (This method is provided in case 
  ///    strong components already computed.)
  void
  assign ( Digraph const& digraph, 
           Components const& components )
          IMPLEMENT(assign(digraph,components));

  /// poset 
  ///   Return the underlying partial order
  Poset const
  poset ( void ) const
          IMPLEMENT(poset());

  /// components 
  ///   Return recurrent components 
  Components const
  components ( void ) const
          IMPLEMENT(components());

};

#include "Dynamics/MorseGraph.h"

/// class MorseGraph public interface
INTERFACE(MorseGraph)  {
  /// assign (Morse Decomposition)
  ///   Assign data to Morse Graph
  template < class SwitchingGraph > void
  assign ( SwitchingGraph const& sg,
           MorseDecomposition const& md ) 
          IMPLEMENT(assign(sg,md));

  /// poset
  ///   Access poset
  Poset const
  poset ( void ) const 
          IMPLEMENT(poset());

  /// annotation
  ///   Return the annotation on vertex v.
  Annotation const
  annotation ( uint64_t v ) const 
          IMPLEMENT(annotation(v));

  /// SHA
  ///   Return a SHA-256 code
  std::string
  SHA256 ( void ) const 
          IMPLEMENT(SHA256());

  /// stringify
  ///   Return a JSON description
  std::string
  stringify ( void ) const 
          IMPLEMENT(stringify());

  /// parse
  ///   Initialize from a JSON description
  void
  parse ( std::string const& str ) 
          IMPLEMENT(parse(str));

};


#include "Parameter/Network.h"
/// Network
///   This class holds network data.
///     * Loads specification files
///     * Outputs Graphviz visualizations
///     * Provides an interface to networks
INTERFACE(Network) {

  /// load
  ///   load from network specification file
  void 
  load ( std::string const& filename )
          IMPLEMENT(load(filename));

  /// size
  ///   Return the number of nodes in the network
  uint64_t
  size ( void ) const
          IMPLEMENT(size());

  /// index
  ///   Return index of node given name string
  uint64_t 
  index ( std::string const& name ) const
          IMPLEMENT(index(name));

  /// name
  ///   Return name of node (given by index)
  std::string const&
  name ( uint64_t index ) const
          IMPLEMENT(name(index));

  /// inputs
  ///   Return a list of inputs to a node
  std::vector<uint64_t> const&
  inputs ( uint64_t index ) const
          IMPLEMENT(inputs(index));

  /// outputs
  ///   Return a list of outputs to a node
  std::vector<uint64_t> const&
  outputs ( uint64_t index ) const
          IMPLEMENT(outputs(index));

  /// logic
  ///   Return the logic of a node (given by index)
  std::vector<std::vector<uint64_t>> const&
  logic ( uint64_t index ) const
          IMPLEMENT(logic(index));

  /// uint64_teraction
  ///   Return the uint64_teraction type of an edge:
  ///   False for repression, true for activation
  bool
  interaction ( uint64_t source, uint64_t target ) const
          IMPLEMENT(interaction(source,target));

  /// order
  ///   Return the out-edge order number of an edge, i.e. so
  ///   outputs(source)[order(source,target)] == target
  uint64_t
  order ( uint64_t source, uint64_t target ) const
          IMPLEMENT(order(source,target));

  /// domains
  ///   Return a list consisting of the number of 
  ///   domains across (i.e. number of out-edges plus one)
  ///   for each dimension (i.e. network node)
  std::vector<uint64_t>
  domains ( void ) const
          IMPLEMENT(domains());

  /// graphviz
  ///   Return a graphviz string (dot language)
  std::string
  graphviz ( std::vector<std::string> const& theme = 
  { "aliceblue", // background color
    "beige",     // node color
    "black", "darkgoldenrod", "blue", "orange", "red", "yellow" // edge group colors
  } ) const
          IMPLEMENT(graphviz(theme));

};

#include "Parameter/LogicParameter.h"

INTERFACE(LogicParameter) {

  /// assign
  ///   Assign data
  void
  assign ( uint64_t n, uint64_t m, std::string const& hex )
          IMPLEMENT(assign(n,m,hex));

  /// operator ()
  ///   Return true if input combination drives the output
  ///   to the right, and false if it drives it to the left
  bool
  operator () ( std::vector<bool> const& input_combination, uint64_t output ) const
          IMPLEMENT(operator()(input_combination,output));

  /// operator ()
  ///   Version of operator () where bit has already been computed
  bool
  operator () ( uint64_t bit ) const
          IMPLEMENT(operator()(bit));

  /// stringify
  ///   Return a JSON-style string
  ///    of form [n,m,"hex"]
  std::string
  stringify ( void ) const
          IMPLEMENT(stringify());

  /// parse
  ///   Initialize from a JSON-style string
  void
  parse ( std::string const& str )
          IMPLEMENT(parse(str));

};

#include "Parameter/OrderParameter.h"

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
  assign ( uint64_t m, uint64_t k )
          IMPLEMENT(assign(m,k));

  /// assign (by permutation)
  ///   Initialize as given permutation
  void
  assign ( std::vector<uint64_t> const& perm )
          IMPLEMENT(assign(perm));

  /// operator ()
  ///   Apply the permutation to the input
  uint64_t 
  operator () ( uint64_t i ) const
          IMPLEMENT(operator()(i));

  /// inverse
  ///   Apply the inverse permuation to input
  uint64_t
  inverse ( uint64_t i ) const
          IMPLEMENT(inverse(i));

  /// permutation ()
  ///   Return the underlying permutation
  std::vector<uint64_t> const&
  permutation ( void ) const
          IMPLEMENT(permutation());

  /// index ()
  ///   Return the index of the permutation
  uint64_t
  index ( void ) const
          IMPLEMENT(index());

  /// size ()
  ///   Return the number of items
  uint64_t
  size ( void ) const
          IMPLEMENT(size());

  /// stringify
  ///   Return a JSON-style string
  ///    of form [p1, p2, p3, ...]
  std::string
  stringify ( void ) const
          IMPLEMENT(stringify());

  /// parse
  ///   Initialize from a JSON-style string
  void
  parse ( std::string const& str )
          IMPLEMENT(parse(str));

};

#include "Phase/Domain.h"

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
public:
  /// Domain
  ///   Construct domain object as
  ///   (0,0,...,0) in 
  ///   {0,1,..,limits[0]-1}x...x{0,1,..,limits[D]-1}
  Domain ( std::vector<uint64_t> const& limits )
          CONSTRUCT(Domain_(limits));

  /// operator []
  ///   Return dth component
  uint64_t 
  operator [] ( uint64_t d ) const
          IMPLEMENT(operator[](d));

  /// operator ++ (preincrement)
  ///   Advance through domain traversal pattern
  Domain & 
  operator ++ ( void )
          CHAIN(operator++());

  /// operator ++ (postincrement)
  ///   Advance through domain traversal pattern
  ///   but return copy of unadvanced domain
  Domain 
  operator ++ ( int )
          { Domain d = *this; ++(*this); return d;}

  /// size
  ///   Return number of dimensions
  uint64_t 
  size ( void ) const
          IMPLEMENT(size());

  /// index
  ///   Return traversal index of domain
  uint64_t 
  index ( void ) const
          IMPLEMENT(index());

  /// setIndex 
  ///   Set the domain by index
  void
  setIndex ( uint64_t i )
          IMPLEMENT(setIndex(i));

  /// left
  ///   Return domain index to the left in dimension d
  ///   (If there is none, behavior is undefined)
  uint64_t
  left ( uint64_t d ) const
          IMPLEMENT(left(d));

  /// right
  ///   Return domain index to the right in dimension d
  ///   (If there is none, behavior is undefined)
  uint64_t
  right ( uint64_t d ) const
          IMPLEMENT(right(d));

  /// right
  /// isMin
  ///   Return true if dth component 
  ///   is 0 (i.e. is minimal)
  bool 
  isMin ( uint64_t d ) const
          IMPLEMENT(isMin(d));

  /// isMax
  ///   Return true if dth component 
  ///   is limits[d]-1 (i.e. is maximal)
  bool 
  isMax ( uint64_t d ) const
          IMPLEMENT(isMax(d));

  /// isValid
  ///   Return true if a valid domain.
  ///   (For use with ++, so one-past-the-end is not valid)
  bool
  isValid ( void ) const
          IMPLEMENT(isValid());

};

#include "Parameter/Parameter.h"

INTERFACE(Parameter) {

  /// attracting
  ///   Return true if domain is attracting.
  ///   This is true iff none of the walls of
  ///   are absorbing.
  bool 
  attracting ( Domain const& dom ) const
          IMPLEMENT(attracting(dom));

  /// absorbing
  ///   Return true if wall is absorbing
  ///   The wall is described by a domain dom,
  ///   a dimension collapse_dim, and a direction. 
  ///   The direction is either -1 or +1, signifying
  ///   the left wall or the right wall with 
  ///   collapse dimension collapse_dim.
  bool
  absorbing ( Domain const& dom, int collapse_dim, int direction ) const
          IMPLEMENT(absorbing(dom,collapse_dim,direction));

  /// assign
  ///   Assign data to parameter
  void
  assign ( std::vector<LogicParameter> const& logic,
           std::vector<OrderParameter> const& order, 
           Network const& network )
          IMPLEMENT(assign(logic,order,network));

  /// assign (network-only)
  ///   Assign network to parameter.
  ///   logic and order left uninitialized.
  void
  assign ( Network const& network )
          IMPLEMENT(assign(network));

  /// network
  ///   Return network
  Network const
  network ( void ) const
          IMPLEMENT(network());

  /// stringify
  ///   Return a JSON description
  std::string
  stringify ( void ) const
          IMPLEMENT(stringify());

  /// parse
  ///   Initialize from a JSON description
  void
  parse ( std::string const& str )
          IMPLEMENT(parse(str));

  /// inequalities
  ///    Output a list of inequalities correspond to the parameter
  std::string
  inequalities ( void ) const
          IMPLEMENT(inequalities());

};

#include "Parameter/ParameterGraph.h"

INTERFACE(ParameterGraph) {

  /// assign
  ///   Assign a network to the parameter graph
  ///   Search in path for logic .dat files
  void
  assign ( Network const& network, 
           std::string const& path = std::string("./data/logic/") )
          IMPLEMENT(assign(network,path));

  /// size
  ///   Return the number of parameters
  uint64_t
  size ( void ) const
          IMPLEMENT(size());

  /// parameter
  ///   Return the parameter associated with an index
  Parameter
  parameter ( uint64_t index ) const
          IMPLEMENT(parameter(index));

  /// index
  ///   Return the index associated with a parameter
  uint64_t
  index ( Parameter const& p ) const
          IMPLEMENT(index(p));

  /// adjacencies
  ///   Return the adjacent parameters to a given parameter
  std::vector<uint64_t>
  adjacencies ( uint64_t index ) const
          IMPLEMENT(adjacencies(index));

  /// network
  ///   Return network
  Network const
  network ( void ) const
          IMPLEMENT(network());

  /// fixedordersize
  ///   Return the number of parameters
  ///   for a fixed ordering
  uint64_t
  fixedordersize ( void ) const
          IMPLEMENT(fixedordersize());

  /// reorderings
  ///   Return of reorderings
  ///   Note: size() = fixedordersize()*reorderings()
  uint64_t
  reorderings ( void ) const
          IMPLEMENT(reorderings());

};

#include "Phase/DomainGraph.h"

INTERFACE(DomainGraph) {
public:
  /// assign
  ///   Construct based on parameter and network
  void
  assign ( Parameter const& parameter )
          IMPLEMENT(assign(parameter));

  /// digraph
  ///   Return underlying digraph
  Digraph const
  digraph ( void ) const  
          IMPLEMENT(digraph());

  /// annotate
  Annotation const
  annotate ( std::vector<uint64_t> const& vertices ) const
          IMPLEMENT(annotate(vertices));

};

#include "Phase/Wall.h"

INTERFACE(Wall) {

  /// Wall 
  ///   Default constructor
  Wall ( void )
          CONSTRUCT(Wall_());

  /// Wall
  ///   Construct from domain collapse (see assign)
  Wall ( Domain const& dom, uint64_t collapse_dim, int direction )
          CONSTRUCT(Wall_(dom,collapse_dim,direction));

  /// assign 
  ///   Create wall from domain, dimension of collapse, 
  ///   and direction of collapse.
  void
  assign ( Domain const& dom, uint64_t collapse_dim, int direction )
          IMPLEMENT(assign(dom,collapse_dim,direction));

  /// index
  ///   Return a unique index. Note wall indices are 
  ///   not guaranteed to be contiguous.
  uint64_t
  index ( void ) const
          IMPLEMENT(index());

};

#include "Phase/WallGraph.h"

INTERFACE(WallGraph) {

  /// assign
  ///   Construct based on parameter and network
  void
  assign ( Parameter const& parameter )
          IMPLEMENT(assign(parameter));

  /// digraph
  ///   Return underlying digraph
  Digraph const
  digraph ( void ) const  
          IMPLEMENT(digraph());

  /// annotate
  Annotation const
  annotate ( std::vector<uint64_t> const& vertices ) const
          IMPLEMENT(annotate(vertices));

};

#endif
