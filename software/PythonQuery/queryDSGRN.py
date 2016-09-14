import DSGRN
import sqlite3
import graphviz

class dsgrnDatabase:
  def __init__(self, database_name):
    """
    Initialize a DSGRN database object
    """
    self.conn = sqlite3.connect(database_name)
    self.network = DSGRN.Network(database_name)
    self.parametergraph = DSGRN.ParameterGraph(self.network)
    # D is the number of network nodes
    self.D = self.parametergraph.dimension()
    self.names = [ self.network.name(i) for i in range(0, self.D)]
    # DSGRN uses an indexing scheme to refer to parameters. It is based on a mixed-radix number scheme
    # where the place value of each digit varies according to the number of logic parameters for each node
    # and the number of order parameter for each node. Specifically, the ordering of the digits is (from least
    # significant) the sizes of each factor graph, followed by the number of permutations of the out-edges for
    # each node. We call these "bases" (as in number base) and we compute the place value for each digit.
    self.indexing_place_bases = [self.parametergraph.logicsize(i) for i in range(0,self.D)] + [self.parametergraph.ordersize(i) for i in range(0,self.D)]
    self.indexing_place_values = reduce ( lambda x, y : x + [x[-1]*y], self.indexing_place_bases[:-1], [1])
    # For single-gene manipulations, we convert from parameter index to (reduced parameter index, gene parameter index)
    # To recover the parameter index from the reduced parameter index, we insert a "digit":
    self.full_parameter_index = lambda rpi, gpi, gene_index : rpi % self.indexing_place_values[gene_index] + gpi * self.indexing_place_values[gene_index] + (rpi // self.indexing_place_values[gene_index]) * self.indexing_place_values[gene_index+1]
    self.reduced_parameter_index = lambda pi, gene_index : (pi % self.indexing_place_values[gene_index] + (pi // self.indexing_place_values[gene_index+1]) * self.indexing_place_values[gene_index], (pi // self.indexing_place_values[gene_index]) % self.indexing_place_bases[gene_index] )
    # To support queries about the having 1, 2, or more than 1 minimal Morse set:
    self.uniquestablemorsegraphindices = frozenset(self.UniqueStableMorseGraphIndices())
    self.bistablemorsegraphindices = frozenset(self.BistableMorseGraphIndices())
    self.multistablemorsegraphindices = frozenset(self.MultistableMorseGraphIndices())

  def __del__(self):
    """
    Commit and close upon destruction
    """
    self.conn.commit()
    self.conn.close()

  def single_gene_query_prepare (self, gene ):
    """
    Add a table to the database to enable gene manipulation queries
    """
    # Sanitize "gene":
    if gene not in self.names:
      raise NameError(gene + " is not the name of a node in the network")
    # "gene_index" gives the integer index used in the representation
    gene_index = self.network.index(gene)
    # N is the size of the factor graph associated with "gene"
    N = self.indexing_place_bases[gene_index]
    # M is the product of the sizes of all remaining factor graphs, and reorderings of all genes (including "gene")
    M = self.parametergraph.size() / N 

    # Now we scan the database and create a new table we can do single gene poset queries with
    c = self.conn.cursor()
    c.execute("drop table if exists " + gene) # gene is sanitized
    c.execute("create table " + gene + " (ReducedParameterIndex INTEGER, GeneParameterIndex INTEGER, ParameterIndex INTEGER, MorseGraphIndex INTEGER)")
    values = [ self.reduced_parameter_index(row[0], gene_index) + row for row in c.execute("select * from Signatures")]
    c.executemany('INSERT INTO ' + gene + ' VALUES (?,?,?,?)', values )
    indexname = gene + '1'
    c.execute('create index ' + gene + '1 on ' + gene + ' (ReducedParameterIndex, GeneParameterIndex)') # gene is sanitized
    self.conn.commit()
    return (N, M)

  def single_gene_query (self, gene, reduced_parameter_index ):
    """
    Return dictionary from hex codes to morse graph indices for single-gene query
    """
    # Sanitize "gene":
    if gene not in self.names:
      raise NameError(gene + " is not the name of a node in the network")
    c = self.conn.cursor()
    c.execute("select GeneParameterIndex,ParameterIndex,MorseGraphIndex from " + gene + " where ReducedParameterIndex=?" , (str(reduced_parameter_index),))
    gene_index = self.network.index(gene)
    factorgraph = self.parametergraph.factorgraph(gene_index)
    return { row[0] : (factorgraph[row[0]],  row[1], row[2]) for row in c }

  def UniqueStableMorseGraphIndices(self):
    """
    Return the list of Morse Graph indices for which the Morse graph has a unique minimal element
    """
    c = self.conn.cursor()
    sqlexpression = "select MorseGraphIndex from (select MorseGraphIndex, count(*) as StableCount from (select MorseGraphIndex,Vertex from MorseGraphVertices except select MorseGraphIndex,Source from MorseGraphEdges) group by MorseGraphIndex) where StableCount=1;"
    return [ row[0] for row in c.execute(sqlexpression) ]

  def BistableMorseGraphIndices(self):
    """
    Return the list of Morse Graph indices for which the Morse graph has precisely two minimal elements
    """
    c = self.conn.cursor()
    sqlexpression = "select MorseGraphIndex from (select MorseGraphIndex, count(*) as StableCount from (select MorseGraphIndex,Vertex from MorseGraphVertices except select MorseGraphIndex,Source from MorseGraphEdges) group by MorseGraphIndex) where StableCount=2;"
    return [ row[0] for row in c.execute(sqlexpression) ]

  def MultistableMorseGraphIndices(self):
    """
    Return the list of Morse Graph indices for which the Morse graph has more than one minimal element
    """
    c = self.conn.cursor()
    sqlexpression = "select MorseGraphIndex from (select MorseGraphIndex, count(*) as StableCount from (select MorseGraphIndex,Vertex from MorseGraphVertices except select MorseGraphIndex,Source from MorseGraphEdges) group by MorseGraphIndex) where StableCount>1;"
    return [ row[0] for row in c.execute(sqlexpression) ]

  def uniquestable_query (self, morsegraphindex):
    """
    Check a Morse Graph for bistability
    """
    return morsegraphindex in self.uniquestablemorsegraphindices

  def bistable_query (self, morsegraphindex):
    """
    Check a Morse Graph for bistability
    """
    return morsegraphindex in self.bistablemorsegraphindices

  def multistable_query (self, morsegraphindex):
    """
    Check a Morse Graph for bistability
    """
    return morsegraphindex in self.multistablemorsegraphindices

  def DrawMorseGraph(self, morsegraphindex):
    """
    Return an object which renders to a graphviz representation in Jupyter
    """
    c = self.conn.cursor()
    sqlexpression = "select Graphviz from MorseGraphViz where MorseGraphIndex = ?"
    c.execute(sqlexpression,(morsegraphindex,))
    gv = c.fetchone()[0]
    print gv 
    return graphviz.Source(gv)
