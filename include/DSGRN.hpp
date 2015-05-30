/// DSGRN.hpp
/// Shaun Harker
/// 2015-05-29
/// This file provides implementations for the user classes of DSGRN
/// Note: each class declared with the "INTERFACE" macro
///       (which is defined in Idiom.h) comes automatically
///       equipped with a friend function operator << 
///       and support for BOOST serialization
#ifndef DSGRN_HPP
#define DSGRN_HPP
#include "Idiom.h"
#include "Database/json.h"
#include "Database/SHA256.h"
#include "Database/sqlambda.h"
#include "DSGRN.h"
#include "Graph/Components.h"
#include "Graph/Digraph.h"
#include "Graph/Poset.h"
#include "Graph/StrongComponents.h"
#include "Dynamics/Annotation.h"
#include "Dynamics/MorseDecomposition.h"
#include "Dynamics/MorseGraph.h"
#include "Parameter/Network.h"
#include "Parameter/LogicParameter.h"
#include "Parameter/OrderParameter.h"
#include "Parameter/Parameter.h"
#include "Parameter/ParameterGraph.h"
#include "Phase/Domain.h"
#include "Phase/DomainGraph.h"
#include "Phase/Wall.h"
#include "Phase/WallGraph.h"
#undef CLASS_SCOPE
#define CLASS_SCOPE Components
  void Components:: CLASS_SCOPE::
  assign ( std::vector<std::shared_ptr<std::vector<uint64_t>>> const& SCCs )
          IMPLEMENT(assign(SCCs));
  CLASS_SCOPE::iterator Components:: CLASS_SCOPE::
  begin ( void ) const
          IMPLEMENT(begin());
  CLASS_SCOPE::iterator Components:: CLASS_SCOPE::
  end ( void ) const
          IMPLEMENT(end());
  uint64_t Components:: CLASS_SCOPE::
  size ( void ) const
          IMPLEMENT(size());
  std::shared_ptr<std::vector<uint64_t>> CLASS_SCOPE::
  operator [] ( uint64_t i ) const
          IMPLEMENT(operator[](i));
  uint64_t CLASS_SCOPE::
  whichComponent ( uint64_t i ) const
          IMPLEMENT(whichComponent(i));
#undef CLASS_SCOPE
#define CLASS_SCOPE Digraph
  std::vector<uint64_t> const& CLASS_SCOPE::
  adjacencies ( uint64_t v ) const
          IMPLEMENT(adjacencies(v));
  std::vector<uint64_t> & CLASS_SCOPE::
  adjacencies ( uint64_t v )
          IMPLEMENT(adjacencies(v));
  uint64_t  CLASS_SCOPE::
  size ( void ) const
          IMPLEMENT(size());
  void CLASS_SCOPE::
  resize ( uint64_t n )
          IMPLEMENT(resize(n));
  uint64_t CLASS_SCOPE::
  add_vertex ( void )
          IMPLEMENT(add_vertex());
  void CLASS_SCOPE::
  add_edge ( uint64_t source, uint64_t target )
          IMPLEMENT(add_edge(source,target));
  std::string CLASS_SCOPE::
  stringify ( void ) const
          IMPLEMENT(stringify());
  void CLASS_SCOPE::
  parse ( std::string const& str )
          IMPLEMENT(parse(str));
#undef CLASS_SCOPE
#define CLASS_SCOPE Poset
  std::vector<uint64_t> const& CLASS_SCOPE::
  adjacencies ( uint64_t v ) const
          IMPLEMENT(adjacencies(v));
  std::vector<uint64_t> & CLASS_SCOPE::
  adjacencies ( uint64_t v )
          IMPLEMENT(adjacencies(v));
  uint64_t  CLASS_SCOPE::
  size ( void ) const
          IMPLEMENT(size());  
  void CLASS_SCOPE::
  resize ( uint64_t n )
          IMPLEMENT(resize(n));
  uint64_t CLASS_SCOPE::
  add_vertex ( void )
          IMPLEMENT(add_vertex());
  void CLASS_SCOPE::
  add_edge ( uint64_t source, uint64_t target )
          IMPLEMENT(add_edge(source,target));
  std::string CLASS_SCOPE::
  stringify ( void ) const
          IMPLEMENT(stringify());
  void CLASS_SCOPE::
  parse ( std::string const& str )
          IMPLEMENT(parse(str));
  void CLASS_SCOPE::
  reduction ( void )
          IMPLEMENT(reduction());
#undef CLASS_SCOPE
#define CLASS_SCOPE Annotation
  uint64_t  CLASS_SCOPE::
  size ( void ) const
          IMPLEMENT(size());
  CLASS_SCOPE::iterator CLASS_SCOPE::
  begin ( void ) const
          IMPLEMENT(begin());
  CLASS_SCOPE::iterator  CLASS_SCOPE::
  end ( void ) const
          IMPLEMENT(end());
  std::string const& CLASS_SCOPE::
  operator [] ( uint64_t i ) const
          IMPLEMENT(operator[](i));
  void CLASS_SCOPE::
  append ( std::string const& label )
          IMPLEMENT(append(label));
  std::string CLASS_SCOPE::
  stringify ( void ) const
          IMPLEMENT(stringify());
  void CLASS_SCOPE::
  parse ( std::string const& str )
          IMPLEMENT(parse(str));
#undef CLASS_SCOPE
#define CLASS_SCOPE MorseDecomposition
  void CLASS_SCOPE::
  assign ( Digraph const& digraph )
          IMPLEMENT(assign(digraph));
  
  void CLASS_SCOPE::
  assign ( Digraph const& digraph, 
           Components const& components )
          IMPLEMENT(assign(digraph,components));
  Poset const CLASS_SCOPE::
  poset ( void ) const
          IMPLEMENT(poset());
  Components const CLASS_SCOPE::
  components ( void ) const
          IMPLEMENT(components());
#undef CLASS_SCOPE
#define CLASS_SCOPE MorseGraph
  template < class SwitchingGraph > void CLASS_SCOPE:: CLASS_SCOPE::
  assign ( SwitchingGraph const& sg,
           MorseDecomposition const& md ) 
          IMPLEMENT(assign(sg,md));
  Poset const CLASS_SCOPE:: CLASS_SCOPE::
  poset ( void ) const 
          IMPLEMENT(poset());
  Annotation const CLASS_SCOPE::
  annotation ( uint64_t v ) const 
          IMPLEMENT(annotation(v));
  std::string CLASS_SCOPE::
  SHA256 ( void ) const 
          IMPLEMENT(SHA256());
  std::string CLASS_SCOPE::
  stringify ( void ) const 
          IMPLEMENT(stringify());
  void CLASS_SCOPE::
  parse ( std::string const& str ) 
          IMPLEMENT(parse(str));
#undef CLASS_SCOPE
#define CLASS_SCOPE Network
  void  CLASS_SCOPE::
  load ( std::string const& filename )
          IMPLEMENT(load(filename));
  uint64_t CLASS_SCOPE::
  size ( void ) const
          IMPLEMENT(size());
  uint64_t  CLASS_SCOPE::
  index ( std::string const& name ) const
          IMPLEMENT(index(name));
  std::string const& CLASS_SCOPE::
  name ( uint64_t index ) const
          IMPLEMENT(name(index));
  std::vector<uint64_t> const& CLASS_SCOPE::
  inputs ( uint64_t index ) const
          IMPLEMENT(inputs(index));
  std::vector<uint64_t> const& CLASS_SCOPE::
  outputs ( uint64_t index ) const
          IMPLEMENT(outputs(index));
  std::vector<std::vector<uint64_t>> const& CLASS_SCOPE::
  logic ( uint64_t index ) const
          IMPLEMENT(logic(index));
  bool CLASS_SCOPE::
  interaction ( uint64_t source, uint64_t target ) const
          IMPLEMENT(interaction(source,target));
  uint64_t CLASS_SCOPE::
  order ( uint64_t source, uint64_t target ) const
          IMPLEMENT(order(source,target));
  
  std::vector<uint64_t> CLASS_SCOPE::
  domains ( void ) const
          IMPLEMENT(domains());
  std::string CLASS_SCOPE::
  graphviz ( std::vector<std::string> const& theme ) const
          IMPLEMENT(graphviz(theme));
#undef CLASS_SCOPE
#define CLASS_SCOPE LogicParameter
  void CLASS_SCOPE::
  assign ( uint64_t n, uint64_t m, std::string const& hex )
          IMPLEMENT(assign(n,m,hex));
  bool CLASS_SCOPE::
  operator () ( std::vector<bool> const& input_combination, uint64_t output ) const
          IMPLEMENT(operator()(input_combination,output));
  bool CLASS_SCOPE::
  operator () ( uint64_t bit ) const
          IMPLEMENT(operator()(bit));
  std::string CLASS_SCOPE::
  stringify ( void ) const
          IMPLEMENT(stringify());
  void CLASS_SCOPE::
  parse ( std::string const& str )
          IMPLEMENT(parse(str));
#undef CLASS_SCOPE
#define CLASS_SCOPE OrderParameter
  void CLASS_SCOPE::
  assign ( uint64_t m, uint64_t k )
          IMPLEMENT(assign(m,k));
  void CLASS_SCOPE::
  assign ( std::vector<uint64_t> const& perm )
          IMPLEMENT(assign(perm));
  uint64_t  CLASS_SCOPE::
  operator () ( uint64_t i ) const
          IMPLEMENT(operator()(i));
  uint64_t CLASS_SCOPE::
  inverse ( uint64_t i ) const
          IMPLEMENT(inverse(i));
  std::vector<uint64_t> const& CLASS_SCOPE::
  permutation ( void ) const
          IMPLEMENT(permutation());
  uint64_t CLASS_SCOPE::
  index ( void ) const
          IMPLEMENT(index());
  uint64_t CLASS_SCOPE::
  size ( void ) const
          IMPLEMENT(size());
  std::string CLASS_SCOPE::
  stringify ( void ) const
          IMPLEMENT(stringify());
  void CLASS_SCOPE::
  parse ( std::string const& str )
          IMPLEMENT(parse(str));
#undef CLASS_SCOPE
#define CLASS_SCOPE Domain
  CLASS_SCOPE::
  Domain ( std::vector<uint64_t> const& limits )
          CONSTRUCT(Domain_(limits));
  uint64_t CLASS_SCOPE::
  operator [] ( uint64_t d ) const
          IMPLEMENT(operator[](d));
  Domain & CLASS_SCOPE::
  operator ++ ( void )
          CHAIN(operator++());
  Domain CLASS_SCOPE::
  operator ++ ( int )
          { Domain d = *this; ++(*this); return d;}
  uint64_t CLASS_SCOPE::
  size ( void ) const
          IMPLEMENT(size());
  uint64_t CLASS_SCOPE::
  index ( void ) const
          IMPLEMENT(index());
  void CLASS_SCOPE::
  setIndex ( uint64_t i )
          IMPLEMENT(setIndex(i));
  uint64_t CLASS_SCOPE::
  left ( uint64_t d ) const
          IMPLEMENT(left(d));
  uint64_t CLASS_SCOPE::
  right ( uint64_t d ) const
          IMPLEMENT(right(d));
  bool  CLASS_SCOPE::
  isMin ( uint64_t d ) const
          IMPLEMENT(isMin(d));
  bool  CLASS_SCOPE::
  isMax ( uint64_t d ) const
          IMPLEMENT(isMax(d));
  bool CLASS_SCOPE::
  isValid ( void ) const
          IMPLEMENT(isValid());
#undef CLASS_SCOPE
#define CLASS_SCOPE Parameter
  bool  CLASS_SCOPE::
  attracting ( Domain const& dom ) const
          IMPLEMENT(attracting(dom));
  bool CLASS_SCOPE::
  absorbing ( Domain const& dom, int collapse_dim, int direction ) const
          IMPLEMENT(absorbing(dom,collapse_dim,direction));
  void CLASS_SCOPE::
  assign ( std::vector<LogicParameter> const& logic,
           std::vector<OrderParameter> const& order, 
           Network const& network )
          IMPLEMENT(assign(logic,order,network));
  void CLASS_SCOPE::
  assign ( Network const& network )
          IMPLEMENT(assign(network));
  Network const CLASS_SCOPE::
  network ( void ) const
          IMPLEMENT(network());
  std::string CLASS_SCOPE::
  stringify ( void ) const
          IMPLEMENT(stringify());
  void CLASS_SCOPE::
  parse ( std::string const& str )
          IMPLEMENT(parse(str));
  std::string CLASS_SCOPE::
  inequalities ( void ) const
          IMPLEMENT(inequalities());
#undef CLASS_SCOPE
#define CLASS_SCOPE ParameterGraph
  void CLASS_SCOPE::
  assign ( Network const& network, 
           std::string const& path )
          IMPLEMENT(assign(network,path));
  uint64_t CLASS_SCOPE::
  size ( void ) const
          IMPLEMENT(size());
  Parameter CLASS_SCOPE::
  parameter ( uint64_t index ) const
          IMPLEMENT(parameter(index));
  uint64_t CLASS_SCOPE::
  index ( Parameter const& p ) const
          IMPLEMENT(index(p));
  std::vector<uint64_t> CLASS_SCOPE::
  adjacencies ( uint64_t index ) const
          IMPLEMENT(adjacencies(index));
  Network const CLASS_SCOPE::
  network ( void ) const
          IMPLEMENT(network());
  uint64_t CLASS_SCOPE::
  fixedordersize ( void ) const
          IMPLEMENT(fixedordersize());
  uint64_t CLASS_SCOPE::
  reorderings ( void ) const
          IMPLEMENT(reorderings());
#undef CLASS_SCOPE
#define CLASS_SCOPE DomainGraph
  void CLASS_SCOPE::
  assign ( Parameter const& parameter )
          IMPLEMENT(assign(parameter));
  Digraph const CLASS_SCOPE::
  digraph ( void ) const  
          IMPLEMENT(digraph());
  Annotation const CLASS_SCOPE::
  annotate ( std::vector<uint64_t> const& vertices ) const
          IMPLEMENT(annotate(vertices));
#undef CLASS_SCOPE
#define CLASS_SCOPE Wall
  CLASS_SCOPE::
  Wall ( void )
          CONSTRUCT(Wall_());
  CLASS_SCOPE::
  Wall ( Domain const& dom, uint64_t collapse_dim, int direction )
          CONSTRUCT(Wall_(dom,collapse_dim,direction));
  void CLASS_SCOPE::
  assign ( Domain const& dom, uint64_t collapse_dim, int direction )
          IMPLEMENT(assign(dom,collapse_dim,direction));
  uint64_t CLASS_SCOPE::
  index ( void ) const
          IMPLEMENT(index());
#undef CLASS_SCOPE
#define CLASS_SCOPE WallGraph
  void CLASS_SCOPE::
  assign ( Parameter const& parameter )
          IMPLEMENT(assign(parameter));
  Digraph const CLASS_SCOPE::
  digraph ( void ) const  
          IMPLEMENT(digraph());
  Annotation const CLASS_SCOPE::
  annotate ( std::vector<uint64_t> const& vertices ) const
          IMPLEMENT(annotate(vertices));
#endif
