/// ParameterSampler.h
/// Shaun Harker
/// 2018-10-31
/// MIT LICENSE

#pragma once 

#include "common.h"

#include "Parameter/Network.h"
#include "Parameter/Parameter.h" 
#include "Parameter/Configuration.h" 

class ParameterSampler {
public:
  ParameterSampler();
  ParameterSampler(Network network);
  ParameterSampler(ParameterGraph pg);
  auto assign(Network) -> void;
  auto assign(ParameterGraph) -> void;
  auto sample(uint64_t pi) const -> std::string;

private:
  Network network;
  ParameterGraph pg;
  std::vector<InstanceLookup> instancelookups;
  std::default_random_engine generator;
  std::uniform_real_distribution<double> distribution(0.0,1.0);

  typedef std::string HexCode;
  typedef std::string Variable;
  typedef std::map<Variable, double> Instance;
  typedef std::map<HexCode, Instance> InstanceLookup;

  /// Gibbs_Sampler
  auto
  Gibbs_Sampler(
    HexCode const& hex, 
    uint64_t n, 
    uint64_t m,
    std::vector<std::vector<uint64_t>> logic,
    Instance const& initial) const
    ->
    Instance;

  /// Name_Parameters
  ///   Given a network, a parameter node, and chosen instances for each network node
  ///   determine the parameter names corresponding to each parameter in the instances
  ///   and construct a Instance suitable for export
  auto
  Name_Parameters(
    Network const& network,
    Parameter const& p,
    std::vector<Instance> const& instances) 
    -> 
    Instance;

  /// InstanceToString
  ///   Make a string from an instance
  auto
  InstanceToString(
    Instance const& instance) const 
    -> 
    std::string;
};

/// Python Bindings

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
namespace py = pybind11;

inline void
ParameterSamplerBinding (py::module &m) {
  py::class_<ParameterSampler, std::shared_ptr<ParameterSampler>>(m, "ParameterSampler")
    .def(py::init<>())
    .def(py::init<Network>())
    .def(py::init<ParameterGraph>())
    .def("sample", &ParameterSampler::sample);
}
