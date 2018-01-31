/// DSGRN.cpp
/// Shaun Harker
/// 2018-01-30
/// MIT LICENSE

#include "DSGRN.hpp"

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
namespace py = pybind11;

PYBIND11_MODULE( _dsgrn, m) {
  // Dynamics
  AnnotationBinding(m);
  MorseDecompositionBinding(m);
  MorseGraphBinding(m);
  // Graph
  // ComponentsBinding(m);
  DigraphBinding(m);
  PosetBinding(m);
  //StrongComponents(m);
  // Parameter
  LogicParameterBinding(m);
  NetworkBinding(m);
  OrderParameterBinding(m);
  ParameterBinding(m);
  ParameterGraphBinding(m);
  ConfigurationBinding(m);
  // Phase
  DomainBinding(m);
  DomainGraphBinding(m);
  // Pattern
  MatchingGraphBinding(m);
  MatchingRelationBinding(m);
  PatternBinding(m);
  PatternGraphBinding(m);
  PatternMatchBinding(m);
  SearchGraphBinding(m);
}
