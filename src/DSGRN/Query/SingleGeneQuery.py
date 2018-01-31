# SingleGeneQuery.py
# MIT LICENSE 2016
# Shaun Harker

from DSGRN.Query.Graph import *
from DSGRN.Query.Hexcodes import *
from DSGRN.Query.Logging import LogToSTDOUT

class SingleGeneQuery:
  def __init__(self, database, gene):
    LogToSTDOUT("SingleGeneQuery(" + str(database.dbname) + ", " + str(gene) + ")")
    self.database = database
    self.gene = gene
    # "gene_index" gives the integer index used in the representation
    self.gene_index = self.database.network.index(self.gene)

    # num_gene_param is the size of the factor graph associated with "gene"
    self.num_gene_param = database.indexing_place_bases[self.gene_index]
    # num_reduced_param is the product of the sizes of all remaining factor graphs, and reorderings of all genes (including "gene")
    self.num_reduced_param = database.parametergraph.size() / self.num_gene_param

    # Create factor graph
    # TODO: The following algorithm for creating a factor graph takes O(NM^2) time, where N is length of hexcodes and M is size of factorgraph.
    #       An O(NM) algorithm is possible (based on looping through codewords and trying out each promotion, using a hash table)
    #       Implement this.
    self.hexcodes = self.database.parametergraph.factorgraph(self.gene_index)
    vertices = set(range(0,len(self.hexcodes)))
    edges = [ (gpi1, gpi2) for gpi1 in vertices for gpi2 in vertices if isAdjacentHexcode(self.hexcodes[gpi1], self.hexcodes[gpi2]) ]
    self.graph = Graph(vertices,edges)
    
    LogToSTDOUT("SingleGeneQuery: FactorGraph generated")

    if not hasattr(database, 'SingleGeneQuery'):
      LogToSTDOUT("SingleGeneQuery: SingleGeneQuery attribute missing from python database object.")
      database.SingleGeneQuery = set()
      # For single-gene manipulations, we convert from parameter index to (reduced parameter index, gene parameter index)
      # To recover the parameter index from the reduced parameter index, we insert a "digit":
      database.full_parameter_index = lambda rpi, gpi, gene_index : rpi % database.indexing_place_values[gene_index] + gpi * database.indexing_place_values[gene_index] + (rpi // database.indexing_place_values[gene_index]) * database.indexing_place_values[gene_index+1]
      database.reduced_parameter_index = lambda pi, gene_index : (pi % database.indexing_place_values[gene_index] + (pi // database.indexing_place_values[gene_index+1]) * database.indexing_place_values[gene_index], (pi // database.indexing_place_values[gene_index]) % database.indexing_place_bases[gene_index] )
    
    LogToSTDOUT("SingleGeneQuery: SingleGeneQuery attributes created.")

    if gene not in database.SingleGeneQuery:
      LogToSTDOUT("SingleGeneQuery: database structure unaware of gene " + str(gene) )
      # Sanitize "gene":
      if gene not in database.names:
        raise NameError(gene + " is not the name of a node in the network")

      LogToSTDOUT("SingleGeneQuery: sanitized " + str(gene) )
      # Now we scan the database and create a new table we can do single gene poset queries with
      c = database.conn.cursor()
      # c.execute("drop table if exists " + gene) # gene is sanitized
      # Check if table exists
      LogToSTDOUT("SingleGeneQuery: cursor constructed " )

      c.execute("SELECT count(*) FROM sqlite_master WHERE type='table' AND name='" + gene + "';")
      LogToSTDOUT("SingleGeneQuery: checked for table" )

      table_exists = c.fetchone()[0]
      if not table_exists:
        LogToSTDOUT("SingleGeneQuery: table did not exist" )
        c.execute("create table " + gene + " (ReducedParameterIndex INTEGER, GeneParameterIndex INTEGER, ParameterIndex INTEGER, MorseGraphIndex INTEGER)")
        LogToSTDOUT("SingleGeneQuery: created table" )

        # # old method -- loads whole table into memory
        # values = [ self.reduced_parameter_index(row[0], self.gene_index) + row for row in c.execute("select * from Signatures")]
        # c.executemany('INSERT INTO ' + gene + ' VALUES (?,?,?,?)', values )

        # new method -- a generator that uses fetchmany to keep memory usage down
        # default value arraysize=10000 was chosen based on limited testing. No improvement seen at 50000 or 100000 on a 13M parameter db. Worse performance at arraysize=1000.
        def valuegenerator (cursor,arraysize=10000):
          while True:
              rows = cursor.fetchmany(arraysize)
              if not rows:
                  break
              yield [database.reduced_parameter_index(row[0], self.gene_index) + row for row in rows]

        c2 = database.conn.cursor() # need a second cursor to keep the place in the iteration as updates to the table occur
        c2.execute("select * from Signatures")
        LogToSTDOUT("SingleGeneQuery: executed select statement on signatures" )
        for values in valuegenerator(c2):
          c.executemany('INSERT INTO ' + gene + ' VALUES (?,?,?,?)', values )
        LogToSTDOUT("SingleGeneQuery: inserted values" )
        c2.close()
        LogToSTDOUT("SingleGeneQuery: closed cursor" )
        c.execute('create index ' + gene + '1 on ' + gene + ' (ReducedParameterIndex, GeneParameterIndex)') # gene is sanitized
        LogToSTDOUT("SingleGeneQuery: created index" )
        database.conn.commit()
        LogToSTDOUT("SingleGeneQuery: committed database transaction" )
      database.SingleGeneQuery.add(gene)
      LogToSTDOUT("SingleGeneQuery: added gene to python database object.")
    LogToSTDOUT("SingleGeneQuery: constructed" )


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
    LogToSTDOUT("SingleGeneQuery(" + str(reduced_parameter_index) + ")")
    c = self.database.execute("select GeneParameterIndex,ParameterIndex,MorseGraphIndex from " + self.gene + " where ReducedParameterIndex=?", (str(reduced_parameter_index),))
    LogToSTDOUT("SingleGeneQuery: SQL statement executed")
    Q = { row[0] : (self.hexcodes[row[0]],  row[1], row[2]) for row in c }
    LogToSTDOUT("SingleGeneQuery: Q constructed")
    graph = self.graph.clone()
    LogToSTDOUT("SingleGeneQuery: graph constructed")
    graph.data = Q
    graph.mgi = lambda gpi : graph.data[gpi][2]
    graph.label = lambda gpi : str(graph.data[gpi][1]) + ':' + str(graph.data[gpi][2])
    graph.num_inputs = len(self.database.network.inputs(self.gene_index))
    graph.num_outputs = len(self.database.network.outputs(self.gene_index))
    graph.essential = lambda gpi : essential(graph.data[gpi][0],graph.num_inputs,graph.num_outputs)
    LogToSTDOUT("SingleGeneQuery: graph attributes emplaced")
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



