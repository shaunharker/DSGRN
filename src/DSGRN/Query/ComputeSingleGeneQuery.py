# ComputeSingleGeneQuery.py
# MIT LICENSE 2017
# Shaun Harker

from DSGRN.Query.Graph import *
from DSGRN.Query.Hexcodes import *
from DSGRN.Query.Logging import LogToSTDOUT

import functools

class ComputeSingleGeneQuery:
  def __init__(self, network, gene, labeller):
    self.network = network
    self.gene = gene
    self.parametergraph = ParameterGraph(self.network)
    # D is the number of network nodes
    self.D = self.parametergraph.dimension()
    self.names = [ self.network.name(i) for i in range(0, self.D)]
    # DSGRN uses an indexing scheme to refer to parameters. It is based on a mixed-radix number scheme
    # where the place value of each digit varies according to the number of logic parameters for each node
    # and the number of order parameter for each node. Specifically, the ordering of the digits is (from least
    # significant) the sizes of each factor graph, followed by the number of permutations of the out-edges for
    # each node. We call these "bases" (as in number base) and we compute the place value for each digit.
    self.indexing_place_bases = [self.parametergraph.logicsize(i) for i in range(0,self.D)] + [self.parametergraph.ordersize(i) for i in range(0,self.D)]
    self.indexing_place_values = functools.reduce ( lambda x, y : x + [x[-1]*y], self.indexing_place_bases[:-1], [1])

    # "gene_index" gives the integer index used in the representation
    self.gene_index = self.network.index(self.gene)
    # num_gene_param is the size of the factor graph associated with "gene"
    self.num_gene_param = self.indexing_place_bases[self.gene_index]
    # num_reduced_param is the product of the sizes of all remaining factor graphs, and reorderings of all genes (including "gene")
    self.num_reduced_param = self.parametergraph.size() / self.num_gene_param

    # Create factor graph
    # TODO: The following algorithm for creating a factor graph takes O(NM^2) time, where N is length of hexcodes and M is size of factorgraph.
    #       An O(NM) algorithm is possible (based on looping through codewords and trying out each promotion, using a hash table)
    #       Implement this.
    self.hexcodes = self.parametergraph.factorgraph(self.gene_index)
    self.vertices = set(range(0,len(self.hexcodes)))
    self.edges = [ (gpi1, gpi2) for gpi1 in self.vertices for gpi2 in self.vertices if isAdjacentHexcode(self.hexcodes[gpi1], self.hexcodes[gpi2]) ]
    self.graph = Graph(self.vertices,self.edges)
    self.full_parameter_index = lambda rpi, gpi, gene_index : rpi % self.indexing_place_values[gene_index] + gpi * self.indexing_place_values[gene_index] + (rpi // self.indexing_place_values[gene_index]) * self.indexing_place_values[gene_index+1]
    self.reduced_parameter_index = lambda pi, gene_index : (pi % self.indexing_place_values[gene_index] + (pi // self.indexing_place_values[gene_index+1]) * self.indexing_place_values[gene_index], (pi // self.indexing_place_values[gene_index]) % self.indexing_place_bases[gene_index] )
    self.labeller = labeller

  def __call__ (self, reduced_parameter_index):
    """
    The query returns a graph which contains the poset of gene parameter indices 
    corresponding to adjusting the parameter by changing the logic parameter associated 
    with the gene being queried. The vertices are labelled according to the function
    labeller which accepts full parameter indices.

    The graph is as follows: 

    * The vertices of the graph are named according to Gene Parameter Index (gpi). 
    * There is a directed edge p -> q iff p < q and the associated logic parameters are adjacent.
    * The graph is labelled (graph.matching_label) with the output of `labeller`

    In addition the following extra structures are provided:

    * `graph.num_inputs` is the number of network edges which are inputs to the gene associated with the query
    * `graph.num_outputs`is the number of network edges which are outputs to the gene associated with the query
    * `graph.essential` is a boolean-valued function which determines if each vertex corresponds to an essential parameter node
    """
    graph = self.graph.clone()
    graph.matching_label = lambda gpi : self.labeller(self.full_parameter_index(reduced_parameter_index,gpi,self.gene_index))
    graph.num_inputs = len(self.network.inputs(self.gene_index))
    graph.num_outputs = len(self.network.outputs(self.gene_index))
    #graph.essential = lambda gpi : essential(graph.data[gpi][0],graph.num_inputs,graph.num_outputs)
    return graph

  def number_of_gene_parameters(self):
    """
    Return number of gene parameters associated with query object
    This is the size of the logic factor graph associated with the selected gene
    """
    return self.num_gene_param

  def number_of_reduced_parameters(self):
    """
    Return number of reduced parameters associated with query object
    This is the size of the parameter grapph divided by the size of the 
    logic factor graph associated with the selected gene
    """  
    return self.num_reduced_param

