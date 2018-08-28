/// ComputeSingleGeneQuery.hpp
/// Shaun Harker
/// 2018-05-14
/// MIT LICENSE

#include "ComputeSingleGeneQuery.h"

namespace ComputeSingleGeneQuery_detail_ {
  inline bool isPowerOfTwo(uint64_t x) { return x && (!(x & (x - 1)));}
  inline bool HammingDistanceOne(uint64_t x, uint64_t y) {return isPowerOfTwo(x ^ y);}
}

inline ComputeSingleGeneQuery::
ComputeSingleGeneQuery(Network network, std::string const& gene, std::function<char(uint64_t)> labeller) {
  using namespace ComputeSingleGeneQuery_detail_;
  self.network = network;
  self.gene = gene;
  self.parametergraph = ParameterGraph(self.network);
  self.D = self.parametergraph.dimension();
  //self.names = [ self.network.name(i) for i in range(0, self.D)]


  // Initialize multi-radix indexing scheme:
  // equivalent python code:
  //   self.indexing_place_bases = [self.parametergraph.logicsize(i) for i in range(0,self.D)] + [self.parametergraph.ordersize(i) for i in range(0,self.D)]
  //   self.indexing_place_values = functools.reduce ( lambda x, y : x + [x[-1]*y], self.indexing_place_bases[:-1], [1])
  
  self.indexing_place_bases.resize(2*self.D);
  self.indexing_place_values.resize(2*self.D);
  uint64_t value = 1;
  for ( uint64_t d = 0; d < self.D; ++ d) {
    auto base = self.parametergraph.logicsize(d);
    self.indexing_place_bases[d]=base;
    self.indexing_place_values[d]=value;
    value *= base;
  }

  for ( uint64_t d = 0; d < self.D; ++ d) {
    auto base = self.parametergraph.ordersize(d);
    self.indexing_place_bases[d+self.D]=base;
    self.indexing_place_values[d+self.D]=value;
    value *= base;
  }

  // "gene_index" gives the integer index used in the representation
  self.gene_index = self.network.index(self.gene);

  // num_gene_param is the size of the factor graph associated with "gene"
  self.num_gene_param = self.indexing_place_bases[self.gene_index];

  // num_reduced_param is the product of the sizes of all remaining factor graphs, and reorderings of all genes (including "gene")
  self.num_reduced_param = self.parametergraph.size() / self.num_gene_param;

  // Create factor graph
  // TODO: The following algorithm for creating a factor graph takes O(NM^2) time, where N is length of hexcodes and M is size of factorgraph.
  //       An O(NM) algorithm is possible (based on looping through codewords and trying out each promotion, using a hash table)
  //       Implement this.
  self.hexcodes = self.parametergraph.factorgraph(self.gene_index);
  self.labeller = labeller;

  // List of contiguous integers for vertices
  uint64_t n = self.hexcodes.size();
  self.vertices.resize(n);
  for ( uint64_t i = 0; i < n; ++ i ) self.vertices[i] = i;

  // Set of edges
  for ( auto gpi1 : self.vertices ) {
    auto x = stoull(self.hexcodes[gpi1], 0, 16 );
    for ( auto gpi2 : self.vertices ) {
      auto y = stoull(self.hexcodes[gpi2], 0, 16 );
      if ( x < y && HammingDistanceOne(x,y)) {
        self.edges.push_back({gpi1, gpi2});
      }
    }
  }
  // Add leaf node by convention (we match on edges, not nodes, so edge to leaf will have label to match last node)
  self.vertices.push_back(n);
  self.edges.push_back({n-1,n});
}

inline uint64_t ComputeSingleGeneQuery::
full_parameter_index(uint64_t rpi, uint64_t gpi) const { 
  return rpi % self.indexing_place_values[self.gene_index] + gpi * self.indexing_place_values[self.gene_index] + 
        (rpi / self.indexing_place_values[self.gene_index]) * self.indexing_place_values[self.gene_index+1];
};

/// reduced_parameter_index
///   Return (reduced_parameter_index, gene_parameter_index)
inline std::pair<uint64_t,uint64_t> ComputeSingleGeneQuery::
reduced_parameter_index(uint64_t pi) const { 
  return {pi % self.indexing_place_values[self.gene_index] + 
         (pi / self.indexing_place_values[self.gene_index+1]) * self.indexing_place_values[self.gene_index], 
         (pi / self.indexing_place_values[self.gene_index]) % self.indexing_place_bases[self.gene_index] };
};


/// operator ()
///   The query returns a graph which contains the poset of gene parameter indices 
///   corresponding to adjusting the parameter by changing the logic parameter associated 
///   with the gene being queried. The vertices are labelled according to the function
///   labeller which accepts full parameter indices.
///   The graph is as follows: 
///     * The vertices of the graph are named according to Gene Parameter Index (gpi). 
///     * There is a directed edge p -> q iff p < q and the associated logic parameters are adjacent.
///     * The graph is labelled (graph.matching_label) with the output of `labeller`
///   In addition the following extra structures are provided:
///     * `graph.num_inputs` is the number of network edges which are inputs to the gene associated with the query
///     * `graph.num_outputs`is the number of network edges which are outputs to the gene associated with the query
///     * `graph.essential` is a boolean-valued function which determines if each vertex corresponds to an essential parameter node
inline NFA ComputeSingleGeneQuery::
operator () (uint64_t reduced_parameter_index) const {
  NFA result;
  for ( auto v : self.vertices ) { 
    auto i = result.add_vertex();
    result.add_edge(i,i,' '); // self epsilon edge
  }
  for ( auto e : self.edges ) { 
    auto u = e.first;
    auto v = e.second;
    auto parameter_index = full_parameter_index(reduced_parameter_index,u);
    auto label = self.labeller(parameter_index);
    result.add_edge(u,v,label);
  }
  result.set_initial(0);
  result.set_final(self.vertices.size()-1);
  return result;
}

/// number_of_gene_parameters
///   Return number of gene parameters associated with query object
///   This is the size of the logic factor graph associated with the selected gene
inline uint64_t ComputeSingleGeneQuery::
number_of_gene_parameters(void) const {
  return self.num_gene_param;
}
/// number_of_reduced_parameters
///   Return number of reduced parameters associated with query object
///   This is the size of the parameter graph divided by the size of the 
///   logic factor graph associated with the selected gene  

inline uint64_t ComputeSingleGeneQuery::
number_of_reduced_parameters(void) const {
  return self.num_reduced_param;
}
