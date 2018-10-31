/// DSGRN.cpp
/// Shaun Harker
/// 2018-01-30
/// MIT LICENSE

#include "DSGRN.hpp"

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
namespace py = pybind11;

PYBIND11_MODULE( _dsgrn, m) {
  TypedObjectBinding(m);
  // Dynamics
  AnnotationBinding(m);
  MorseDecompositionBinding(m);
  MorseGraphBinding(m);
  // Graph
  DigraphBinding(m);
  PosetBinding(m);
  ComponentsBinding(m);
  StrongComponentsBinding(m);
  LabelledMultidigraphBinding(m);
  // Parameter
  LogicParameterBinding(m);
  NetworkBinding(m);
  OrderParameterBinding(m);
  ParameterBinding(m);
  ParameterGraphBinding(m);
  ConfigurationBinding(m);
  ParameterSamplerBinding(m);
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
  // Query
  NFABinding(m);
  ComputeSingleGeneQueryBinding(m);
  ThompsonsConstructionBinding(m);
}
