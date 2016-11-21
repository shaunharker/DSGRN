# SingleGeneQuery.py
# MIT LICENSE 2016
# Shaun Harker

from Graph import *
from Hexcodes import *

class SingleGeneQuery:
  def __init__(self, database, gene):
    self.database = database
    self.gene = gene
    # "gene_index" gives the integer index used in the representation
    gene_index = database.network.index(gene)
    # num_gene_param is the size of the factor graph associated with "gene"
    self.num_gene_param = database.indexing_place_bases[gene_index]
    # num_reduced_param is the product of the sizes of all remaining factor graphs, and reorderings of all genes (including "gene")
    self.num_reduced_param = database.parametergraph.size() / self.num_gene_param
    if not hasattr(database, 'SingleGeneQuery'):
      database.SingleGeneQuery = set()
      # For single-gene manipulations, we convert from parameter index to (reduced parameter index, gene parameter index)
      # To recover the parameter index from the reduced parameter index, we insert a "digit":
      database.full_parameter_index = lambda rpi, gpi, gene_index : rpi % database.indexing_place_values[gene_index] + gpi * database.indexing_place_values[gene_index] + (rpi // database.indexing_place_values[gene_index]) * database.indexing_place_values[gene_index+1]
      database.reduced_parameter_index = lambda pi, gene_index : (pi % database.indexing_place_values[gene_index] + (pi // database.indexing_place_values[gene_index+1]) * database.indexing_place_values[gene_index], (pi // database.indexing_place_values[gene_index]) % database.indexing_place_bases[gene_index] )
    if gene not in database.SingleGeneQuery:
      # Sanitize "gene":
      if gene not in database.names:
        raise NameError(gene + " is not the name of a node in the network")

      # Now we scan the database and create a new table we can do single gene poset queries with
      c = database.conn.cursor()
      c.execute("drop table if exists " + gene) # gene is sanitized
      c.execute("create table " + gene + " (ReducedParameterIndex INTEGER, GeneParameterIndex INTEGER, ParameterIndex INTEGER, MorseGraphIndex INTEGER)")

      # # old method -- loads whole table into memory
      # values = [ self.reduced_parameter_index(row[0], gene_index) + row for row in c.execute("select * from Signatures")]
      # c.executemany('INSERT INTO ' + gene + ' VALUES (?,?,?,?)', values )

      # new method -- a generator that uses fetchmany to keep memory usage down
      # default value arraysize=10000 was chosen based on limited testing. No improvement seen at 50000 or 100000 on a 13M parameter db. Worse performance at arraysize=1000.
      def valuegenerator (cursor,arraysize=10000):
        while True:
            rows = cursor.fetchmany(arraysize)
            if not rows:
                break
            yield [database.reduced_parameter_index(row[0], gene_index) + row for row in rows]

      c2 = database.conn.cursor() # need a second cursor to keep the place in the iteration as updates to the table occur
      c2.execute("select * from Signatures")
      for values in valuegenerator(c2):
        c.executemany('INSERT INTO ' + gene + ' VALUES (?,?,?,?)', values )
      c2.close()
      c.execute('create index ' + gene + '1 on ' + gene + ' (ReducedParameterIndex, GeneParameterIndex)') # gene is sanitized
      database.conn.commit()
      database.SingleGeneQuery.add(gene)

  def __call__ (self, reduced_parameter_index):
    """
    The query returns a graph which contains the poset of gene parameter indices 
    corresponding to adjusting the parameter by changing the logic parameter associated 
    with the gene being queried.
    The graph is as follows: 

    * The vertices of the graph are named according to Gene Parameter Index (gpi). 
    * There is a directed edge p -> q iff p < q and the associated logic parameters are adjacent.
    * The graph is labelled with pairs (Parameter index, Morse graph index).

    In addition the following extra structures are provided:

    * `graph.data` is a dictionary from gene parameter index to (hex code, parameter index, morse graph index)
    * `graph.mgi` is a function which accepts a gpi and returns the associated Morse graph index
    * `graph.num_inputs` is the number of network edges which are inputs to the gene associated with the query
    * `graph.num_outputs`is the number of network edges which are outputs to the gene associated with the query
    * `graph.essential` is a boolean-valued function which determines if each vertex corresponds to an essential parameter node
    """
    c = self.database.conn.cursor()
    c.execute("select GeneParameterIndex,ParameterIndex,MorseGraphIndex from " + self.gene + " where ReducedParameterIndex=?" , (str(reduced_parameter_index),))
    gene_index = self.database.network.index(self.gene)
    factorgraph = self.database.parametergraph.factorgraph(gene_index)
    Q = { row[0] : (factorgraph[row[0]],  row[1], row[2]) for row in c }
    vertices = set(Q.keys())
    edges = [ (gpi1, gpi2) for gpi1 in Q.keys() for gpi2 in Q.keys() if isAdjacentHexcode(Q[gpi1][0], Q[gpi2][0]) ]
    graph = Graph(vertices,edges)
    graph.data = Q
    graph.mgi = lambda gpi : graph.data[gpi][2]
    graph.label = lambda gpi : str(graph.data[gpi][1]) + ':' + str(graph.data[gpi][2])
    graph.num_inputs = len(self.database.network.inputs(gene_index))
    graph.num_outputs = len(self.database.network.outputs(gene_index))
    graph.essential = lambda gpi : essential(graph.data[gpi][0],graph.num_inputs,graph.num_outputs)
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



