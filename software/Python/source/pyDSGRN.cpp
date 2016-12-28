
#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>

#include "DSGRN.h"

#include <vector>

// Network wrapping incidentals

// IntList 
typedef std::vector<uint64_t> IntList;
std::string
printIntList ( IntList const* self ) {
  std::stringstream ss;
  bool first = true;
  ss << "[";
  for ( auto const& item : *self ) {
    if ( first ) first = false; else ss << ", ";
    ss << item;
  }
  ss << "]";
  return ss.str();
}

// IntListList
typedef std::vector<std::vector<uint64_t>> IntListList;
std::string
printIntListList ( IntListList const* self ) {
  std::stringstream ss;
  bool first = true;
  ss << "[";
  for ( auto const& item : *self ) {
    if ( first ) first = false; else ss << ", ";
    ss << printIntList(&item);
  }
  ss << "]";
  return ss.str();
}

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(network_overloads, graphviz, 0, 1)

// LogicParameter wrapping incidentals

//BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(LogicParameter_overloads, operator(), 0, 2)

typedef std::vector<LogicParameter> LogicParameterList;
std::string
printLogicParameterList( LogicParameterList const* self ) {
  std::stringstream ss;
  bool first = true;
  ss << "[";
  for ( auto const& item : *self ) {
    if ( first ) first = false; else ss << ", ";
    ss << item.stringify();
  }
  ss << "]";
  return ss.str();
}

// OrderParameter wrapping incidentals
typedef std::vector<OrderParameter> OrderParameterList;
std::string
printOrderParameterList( OrderParameterList const* self ) {
  std::stringstream ss;
  bool first = true;
  ss << "[";
  for ( auto const& item : *self ) {
    if ( first ) first = false; else ss << ", ";
    ss << item.stringify();
  }
  ss << "]";
  return ss.str();
}

// ParameterGraph wrapping incidentals

typedef std::vector<std::string> StringList;
std::string
printStringList( StringList const* self ) {
  std::stringstream ss;
  bool first = true;
  ss << "[";
  for ( auto const& item : *self ) {
    if ( first ) first = false; else ss << ", ";
    ss << item;
  }
  ss << "]";
  return ss.str();
}

// Domain wrapping incidentals
std::string
printDomain ( Domain const* domain ) {
  std::stringstream ss;
  ss << *domain;
  return ss . str ();
}

// Poset wrapping incidentals
typedef std::set<uint64_t> IntSet;

// MorseDecomposition wrapping incidentals
std::string
printMorseDecomposition ( MorseDecomposition const* md ) {
  std::stringstream ss;
  ss << *md;
  return ss . str ();
}

// MatchingGraph wrapping incidentals
typedef std::pair<uint64_t,uint64_t> IntPair;

std::string
printIntPair ( IntPair const* self ) {
  std::stringstream ss;
  bool first = true;
  ss << "(" << self -> first << ", " << self -> second << ")";
  return ss.str();
}

typedef std::vector<IntPair> IntPairList;

std::string
printIntPairList ( IntPairList const* self ) {
  std::stringstream ss;
  bool first = true;
  ss << "[";
  for ( auto const& item : *self ) {
    if ( first ) first = false; else ss << ", ";
    ss << printIntPair(&item);
  }
  ss << "]";
  return ss.str();
}

// 
BOOST_PYTHON_MODULE(libpyDSGRN)
{
  using namespace boost::python;

  // Network

  class_<IntList>("IntList")
    .def(vector_indexing_suite<IntList>() )
    .def("__str__", &printIntList)
    .def("__repr__", &printIntList)
  ;

  class_<IntListList>("IntListList")
    .def(vector_indexing_suite<IntListList>() )
    .def("__str__", &printIntListList)
    .def("__repr__", &printIntListList)
  ;

  class_<Network>("Network", init<std::string>())
    .def(init<>())
    .def("load", &Network::load)  
    .def("assign", &Network::assign)  
    .def("size", &Network::size)  
    .def("index", &Network::index)  
    .def("name", &Network::name, return_value_policy<copy_const_reference>()) 
    .def("inputs", &Network::inputs, return_value_policy<copy_const_reference>()) 
    .def("outputs", &Network::inputs, return_value_policy<copy_const_reference>()) 
    .def("logic", &Network::logic, return_value_policy<copy_const_reference>())
    .def("essential", &Network::essential) 
    .def("interaction", &Network::interaction) 
    .def("order", &Network::order) 
    .def("domains", &Network::domains) 
    .def("specification", &Network::specification)
    .def("graphviz", &Network::graphviz, network_overloads()) 
    .def("__str__", &Network::specification)
  ;

  // LogicParameter

  class_<LogicParameterList>("LogicParameterList")
    .def(vector_indexing_suite<LogicParameterList>() )
    .def("__str__", &printLogicParameterList)
    .def("__repr__", &printLogicParameterList)
  ; // Note: to get this to work I needed to make LogicParameter == comparable in C++ code

  class_<LogicParameter>("LogicParameter", init<uint64_t, uint64_t, std::string>())
    .def(init<>())
    .def("assign", &LogicParameter::assign)  
    .def("__call__",static_cast<bool (LogicParameter::*)( std::vector<bool> const&, uint64_t ) const>
       (&LogicParameter::operator()) ) // How is python user going to get hands on vector<bool> ? TODO
    .def("__call__",static_cast<bool (LogicParameter::*)( uint64_t ) const>
       (&LogicParameter::operator()) )
    .def("bin", &LogicParameter::bin)
    .def("stringify", &LogicParameter::stringify)
    .def("parse", &LogicParameter::parse)
    .def("hex", &LogicParameter::hex, return_value_policy<copy_const_reference>())
    .def("adjacencies", &LogicParameter::adjacencies)
    .def("__str__", &LogicParameter::stringify)
  ;

  // OrderParameter

  class_<OrderParameterList>("OrderParameterList")
    .def(vector_indexing_suite<OrderParameterList>() )
    .def("__str__", &printOrderParameterList)
    .def("__repr__", &printOrderParameterList)
  ; // Note: to get this to work I needed to make OrderParameter == comparable in C++ code

  class_<OrderParameter>("OrderParameter", init<>())
    .def(init<uint64_t,uint64_t>())
    .def(init<std::vector<uint64_t>const&>()) // TODO
    .def("assign", static_cast<void (OrderParameter::*)( uint64_t, uint64_t )>(&OrderParameter::assign) )
    .def("assign", static_cast<void (OrderParameter::*)( std::vector<uint64_t> const&)>(&OrderParameter::assign) )
    .def("__call__", &OrderParameter::operator() )
    .def("inverse", &OrderParameter::inverse )
    .def("permutation", &OrderParameter::permutation, return_value_policy<copy_const_reference>())
    .def("index", &OrderParameter::index)
    .def("size", &OrderParameter::size)
    .def("stringify", &OrderParameter::stringify)
    .def("parse", &OrderParameter::parse)
    .def("adjacencies", &OrderParameter::adjacencies)
    .def("__str__", &OrderParameter::stringify)
  ;

  // Parameter

  class_<Parameter>("Parameter", init<>())
    .def(init<std::vector<LogicParameter> const&, std::vector<OrderParameter> const&, Network const& >())
    .def(init<Network const&>())
    .def("assign", static_cast<void (Parameter::*)(std::vector<LogicParameter> const&, std::vector<OrderParameter> const&, Network const& network)>(&Parameter::assign) )
    .def("assign", static_cast<void (Parameter::*)(Network const& )>(&Parameter::assign) )
    .def("attracting", &Parameter::attracting)
    .def("asborbing", &Parameter::absorbing)
    .def("regulator", &Parameter::regulator)
    .def("labelling", &Parameter::labelling)
    .def("network", &Parameter::network)
    .def("stringify", &Parameter::stringify)
    .def("parse", &Parameter::parse)
    .def("inequalities", &Parameter::inequalities)
    .def("logic", &Parameter::logic, return_value_policy<copy_const_reference>())
    .def("order", &Parameter::order, return_value_policy<copy_const_reference>())
    .def("__str__", &Parameter::stringify)
  ;

  // ParameterGraph

  class_<StringList>("StringList")
    .def(vector_indexing_suite<StringList>() )
    .def("__str__", &printStringList)
    .def("__repr__", &printStringList)
  ;

  class_<ParameterGraph>("ParameterGraph", init<>())
    .def(init<Network const&>())
    .def("assign", &ParameterGraph::assign)
    .def("size", &ParameterGraph::size)
    .def("dimension", &ParameterGraph::dimension)
    .def("logicsize", &ParameterGraph::logicsize)
    .def("ordersize", &ParameterGraph::ordersize)
    .def("factorgraph", &ParameterGraph::factorgraph, return_value_policy<copy_const_reference>())
    .def("parameter", &ParameterGraph::parameter)
    .def("index", &ParameterGraph::index)
    .def("adjacencies", &ParameterGraph::adjacencies)
    .def("network", &ParameterGraph::network)
    .def("fixedordersize", &ParameterGraph::fixedordersize)
    .def("reorderings", &ParameterGraph::reorderings)
  ;

  // Digraph

  class_<Digraph>("Digraph", init<>())
    .def(init<std::vector<std::vector<uint64_t>>&>())
    .def("assign", &Digraph::assign)
    .def("adjacencies", static_cast<std::vector<uint64_t> const& (Digraph::*)(uint64_t)const>(&Digraph::adjacencies), return_value_policy<copy_const_reference>())
    .def("size", &Digraph::size)
    .def("resize", &Digraph::resize)
    .def("add_vertex", &Digraph::add_vertex)
    .def("add_edge", &Digraph::add_edge)
    .def("finalize", &Digraph::finalize)
    .def("transpose", &Digraph::transpose)
    .def("transitive_reduction", &Digraph::transitive_reduction)
    .def("transitive_closure", &Digraph::transitive_closure)
    .def("permute", &Digraph::permute)
    .def("stringify", &Digraph::stringify)
    .def("parse", &Digraph::parse)
    .def("graphviz", &Digraph::graphviz)
    .def("__str__", &Digraph::graphviz)
  ;

  // Poset

  class_<IntSet>("IntSet")  // return type, argument type of "Poset::maximal"
    .def(init<>() )
  ;

  class_<Poset>("Poset", init<>())
    .def(init<std::vector<std::vector<uint64_t>>&>())
    .def(init<Digraph const&>())
    .def("assign", static_cast<void(Poset::*)(std::vector<std::vector<uint64_t>> &)>(&Poset::assign))
    .def("assign", static_cast<void(Poset::*)(Digraph const&)>(&Poset::assign))
    .def("size", &Poset::size)
    .def("parents", &Poset::parents, return_value_policy<copy_const_reference>())
    .def("children", &Poset::children, return_value_policy<copy_const_reference>())
    .def("ancestors", &Poset::ancestors, return_value_policy<copy_const_reference>())
    .def("descendants", &Poset::descendants, return_value_policy<copy_const_reference>())
    .def("maximal", &Poset::maximal)
    .def("compare", &Poset::compare)
    .def("permute", &Poset::permute)
    .def("stringify", &Poset::stringify)
    .def("parse", &Poset::parse)
    .def("graphviz", &Poset::graphviz)
    .def("__str__", &Poset::stringify)
  ;

  // Domain
  // TODO: document interface differences between python/C++ this wrapper entails
  class_<Domain>("Domain", init<>())
    .def(init<std::vector<uint64_t> const&>())
    .def("assign", &Domain::assign)
    .def("__getitem__", &Domain::operator[])
    .def("__len__", &Domain::size)
    .def("size", &Domain::size)
    .def("next", static_cast<Domain& (Domain::*)(void)>(&Domain::operator++), return_value_policy<reference_existing_object>() )
    .def("index", &Domain::index)
    .def("setIndex", &Domain::setIndex)
    .def("left", &Domain::left)
    .def("right", &Domain::right)
    .def("isMin", &Domain::isMin)
    .def("isMax", &Domain::isMax)
    .def("isValid", &Domain::isValid)
    .def("__str__", &printDomain)
  ;

  // DomainGraph
  class_<DomainGraph>("DomainGraph", init<>())
    .def(init<Parameter const>())
    .def("digraph", &DomainGraph::digraph)
    .def("dimension", &DomainGraph::dimension)
    .def("coordinates", &DomainGraph::coordinates)
    .def("label", static_cast<uint64_t(DomainGraph::*)(uint64_t)const>(&DomainGraph::label))
    .def("label", static_cast<uint64_t(DomainGraph::*)(uint64_t,uint64_t)const>(&DomainGraph::label))
    .def("direction", &DomainGraph::direction)
    .def("regulator", &DomainGraph::regulator)
    .def("annotate", &DomainGraph::annotate)
    .def("graphviz", &DomainGraph::graphviz)
  ;

  // MorseDecomposition
  class_<MorseDecomposition>("MorseDecomposition", init<>())
    .def(init<Digraph const&>())
    .def(init<Digraph const&, Components const&>())
    .def("assign", static_cast<void(MorseDecomposition::*)(Digraph const&)>(&MorseDecomposition::assign))
    .def("assign", static_cast<void(MorseDecomposition::*)(Digraph const&, Components const&)>(&MorseDecomposition::assign))
    .def("poset", &MorseDecomposition::poset)
    .def("components", &MorseDecomposition::components)
    .def("recurrent", &MorseDecomposition::recurrent)
    .def("morseset", &MorseDecomposition::morseset)
    .def("graphviz", &MorseDecomposition::graphviz)
    .def("__str__", &printMorseDecomposition)
  ;

  // MorseGraph
  class_<MorseGraph>("MorseGraph", init<>())
    .def("assign", &MorseGraph::assign<DomainGraph>)
    .def("poset", &MorseGraph::poset)
    //.def("annotation", &MorseGraph::annotation)
    .def("SHA256", &MorseGraph::SHA256)
    .def("stringify", &MorseGraph::stringify)
    .def("parse", &MorseGraph::parse)
    .def("graphviz", &MorseGraph::graphviz)
    .def("__str__", &MorseGraph::stringify)
  ;

  // Components
  //class_<Components>("Components")
  //  .def(vector_indexing_suite<Components>() )
    //.def("__str__", &printComponents)
    //.def("__repr__", &printComponents)
  //;

  // ComponentContainer
  //class_<Components::ComponentContainer>("ComponentContainer")
  //  .def(vector_indexing_suite<Components::ComponentContainer>() )
    //.def("__str__", &printComponentContainer)
    //.def("__repr__", &printComponentContainer
  //;

  // Component
  //class_<Component>("Component")
  //  .def(vector_indexing_suite<Component>() )
    //.def("__str__", &printComponent)
    //.def("__repr__", &printComponent)
  //;

  // Pattern

  class_<Pattern>("Pattern", init<>())
    .def(init<Poset const&, std::vector<uint64_t> const&, uint64_t, uint64_t>())
    .def("assign", &Pattern::assign)
    .def("load", &Pattern::load)
    .def("label", &Pattern::label)
    .def("dimension", &Pattern::dimension)
    .def("poset", &Pattern::poset)
    .def("event", &Pattern::event)
    .def("stringify", &Pattern::stringify)
    .def("parse", &Pattern::parse)
  ;  

  // PatternGraph

  class_<PatternGraph>("PatternGraph", init<>())
    .def(init<Pattern const&>())
    .def("assign", &PatternGraph::assign)
    .def("root", &PatternGraph::root)
    .def("leaf", &PatternGraph::leaf)
    .def("size", &PatternGraph::size)
    .def("dimension", &PatternGraph::dimension)
    .def("label", &PatternGraph::label)
    .def("consume", &PatternGraph::consume)
    .def("graphviz", &PatternGraph::graphviz)
    .def("graphviz_with_highlighted_path", &PatternGraph::graphviz_with_highlighted_path)
  ;  

  // SearchGraph

  class_<SearchGraph>("SearchGraph", init<>())
    .def(init<DomainGraph>())
    .def(init<DomainGraph, uint64_t>())
    .def(init<std::vector<uint64_t> const&, uint64_t>())
    .def("assign", static_cast<void(SearchGraph::*)(DomainGraph)>(&SearchGraph::assign))
    .def("assign", static_cast<void(SearchGraph::*)(DomainGraph, uint64_t)>(&SearchGraph::assign))
    .def("assign", static_cast<void(SearchGraph::*)(std::vector<uint64_t> const&, uint64_t)>(&SearchGraph::assign))
    .def("size", &SearchGraph::size)
    .def("dimension", &SearchGraph::dimension)
    .def("label", &SearchGraph::label)
    .def("adjacencies", &SearchGraph::adjacencies, return_value_policy<copy_const_reference>())
    .def("event", &SearchGraph::event)
    .def("graphviz", &SearchGraph::graphviz)
    .def("graphviz_with_highlighted_path", &SearchGraph::graphviz_with_highlighted_path)
    .def("vertexInformation", &SearchGraph::vertexInformation)
    .def("edgeInformation", &SearchGraph::edgeInformation)
  ;  

  // MatchingRelation

  class_<MatchingRelation>("MatchingRelation", init<>())
    .def(init<uint64_t>())
    .def("assign", &MatchingRelation::assign)
    .def("dimension", &MatchingRelation::dimension)
    .def("vertex_labelstring", &MatchingRelation::vertex_labelstring)
    .def("edge_labelstring", &MatchingRelation::edge_labelstring)
  ;  

  // MatchingGraph

  class_<IntPair>("IntPair")
    .def_readwrite("first", &IntPair::first)
    .def_readwrite("second", &IntPair::second)
    .def("__str__", &printIntPair)
    .def("__repr__", &printIntPair)
  ;

  class_<IntPairList>("IntPairList")
    .def(vector_indexing_suite<IntPairList>() )
    .def("__str__", &printIntPairList)
    .def("__repr__", &printIntPairList)
  ;

  class_<MatchingGraph>("MatchingGraph", init<>())
    .def(init<SearchGraph const&, PatternGraph const&>())
    .def("assign", &MatchingGraph::assign)
    .def("searchgraph", &MatchingGraph::searchgraph, return_value_policy<copy_const_reference>())
    .def("patterngraph", &MatchingGraph::patterngraph, return_value_policy<copy_const_reference>())
    .def("query", &MatchingGraph::query)
    .def("adjacencies", &MatchingGraph::adjacencies)
    .def("roots", &MatchingGraph::roots)
    .def("domain", &MatchingGraph::domain)
    .def("position", &MatchingGraph::position)
    .def("vertex", &MatchingGraph::vertex)
    .def("graphviz", &MatchingGraph::graphviz)
    .def("graphviz_with_highlighted_path", &MatchingGraph::graphviz_with_highlighted_path)
  ;  

  // PatternMatch

  def("QueryCycleMatch", QueryCycleMatch);
  def("QueryPathMatch", QueryPathMatch);
  def("CycleMatch", CycleMatch);
  def("PathMatch", PathMatch);

}
