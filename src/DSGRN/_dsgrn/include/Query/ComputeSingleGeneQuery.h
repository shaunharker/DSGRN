/// ComputeSingleGeneQuery.h
/// Shaun Harker
/// 2018-05-14
/// MIT LICENSE

#pragma once

#include "common.h"

#include "Parameter/Network.h"
#include "Parameter/ParameterGraph.h"
#include "Query/NFA.h"

struct ComputeSingleGeneQuery_ {
  Network network;
  ParameterGraph parametergraph;
  uint64_t D;     // D is the number of network nodes
  //std::vector<std::string> names;   // Names of nodes
  
  // Distinguished gene
  std::string gene;
  uint64_t gene_index;
  uint64_t num_gene_param;
  uint64_t num_reduced_param;

  // DSGRN uses an indexing scheme to refer to parameters. It is based on a mixed-radix number scheme
  // where the place value of each digit varies according to the number of logic parameters for each node
  // and the number of order parameter for each node. Specifically, the ordering of the digits is (from least
  // significant) the sizes of each factor graph, followed by the number of permutations of the out-edges for
  // each node. We call these "bases" (as in number base) and we compute the place value for each digit.
  std::vector<uint64_t> indexing_place_bases;
  std::vector<uint64_t> indexing_place_values;

  std::vector<std::string> hexcodes;
  std::vector<uint64_t> vertices;
  std::vector<std::pair<uint64_t,uint64_t>> edges;

  std::function<char(uint64_t)> labeller;
};

class ComputeSingleGeneQuery {
public:
  ComputeSingleGeneQuery(Network network, std::string const& gene, std::function<char(uint64_t)> labeller);

  /// operator ()
  ///   The query returns a NFA which recognizes paths through the Hasse diagram of the poset of gene parameter indices 
  ///   corresponding to adjusting the parameter by changing the logic parameter associated 
  ///   with the gene being queried. The vertices are labelled according to the function
  ///   labeller which accepts full parameter indices.
  ///   The graph is as follows: 
  ///     * The vertices of the graph are named according to Gene Parameter Index (gpi). 
  ///     * There is a directed edge p -> q iff p < q and the associated logic parameters are adjacent.
  ///     * The graph is labelled with the output of `labeller`
  NFA
  operator () (uint64_t reduced_parameter_index) const;

  /// full_parameter_index
  ///   Given reduced parameter index and gene parameter index (rpi, gpi)
  ///   create full parameter index (pi)
  uint64_t
  full_parameter_index(uint64_t rpi, uint64_t gpi) const;

  /// reduced_parameter_index
  ///   Given full parameter index (pi)
  ///   Return reduced_parameter_index and gene_parameter_index (rpi, gpi)
  std::pair<uint64_t,uint64_t>
  reduced_parameter_index(uint64_t pi) const;


  /// number_of_gene_parameters
  ///   Return number of gene parameters associated with query object
  ///   This is the size of the logic factor graph associated with the selected gene
  uint64_t
  number_of_gene_parameters(void) const;

  /// number_of_reduced_parameters
  ///   Return number of reduced parameters associated with query object
  ///   This is the size of the parameter grapph divided by the size of the 
  ///   logic factor graph associated with the selected gene  
  uint64_t
  number_of_reduced_parameters(void) const;

private:
  ComputeSingleGeneQuery_ self;
};

/// Python Bindings

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>

namespace py = pybind11;

inline void
ComputeSingleGeneQueryBinding (py::module &m) {
  py::class_<ComputeSingleGeneQuery, std::shared_ptr<ComputeSingleGeneQuery>>(m, "NewComputeSingleGeneQuery")
    .def(py::init<Network, std::string const&, std::function<char(uint64_t)>>())
    .def("__call__", &ComputeSingleGeneQuery::operator())
    .def("full_parameter_index",&ComputeSingleGeneQuery::full_parameter_index)
    .def("reduced_parameter_index",&ComputeSingleGeneQuery::reduced_parameter_index)
    .def("number_of_gene_parameters", &ComputeSingleGeneQuery::number_of_gene_parameters)
    .def("number_of_reduced_parameters", &ComputeSingleGeneQuery::number_of_reduced_parameters);
}

