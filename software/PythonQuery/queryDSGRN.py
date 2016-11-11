from __future__ import generators

import DSGRN
import sqlite3
import graphviz

class Graph:
  def __init__(self, vertices, edges):
    self.vertices = vertices
    self.edges = edges 
    self.adjacency_lists = {}
    for s,t in self.edges:
      if not s in self.vertices:
        self.vertices.add(s)
      if not t in self.vertices:
        self.vertices.add(t)
      if not s in self.adjacency_lists:
          self.adjacency_lists[s] = []
      self.adjacency_lists[s].append(t) 

  def _repr_svg_(self):
    return graphviz.Source('digraph {' + \
  '\n'.join([ 'X' + str(v) + '[label="' + self.label(v) + '";style="filled";fillcolor="' + self.color(v) + '"];' for v in self.vertices ]) + \
   '\n' + '\n'.join([ 'X' + str(u) + " -> " + 'X' + str(v) + ';' for (u, v) in self.edges ]) + \
   '\n' + '}\n')._repr_svg_()

  def numberOfPaths(self, p, q):
    """
    Compute the number of paths from "p" to "q" in the graph described by "adjacency_lists"
    """
    cache = {q : 1}
    subresult = lambda x : cache.get(x, sum([subresult(y) for y in self.adjacency_lists.get(x,[])]))
    return subresult(p)

  def subgraph(self, predicate):
    """
    Extract induced subgraph consisting of vertices satisfying "predicate"
    """
    return Graph({ v for v in self.vertices if predicate(v)}, { (s,t) for (s,t) in self.edges if predicate(s) and predicate(t)})

  def unavoidable(self, p, q, predicate):
    """
    Return "true" if every path in a graph from "p" to "q" must satisfy "predicate" at one or more vertices.
    """
    return self.subgraph(lambda v : not predicate(v)).numberOfPaths(p,q) == 0

  def label(self, v):
    """
    Return a label string to be used when displaying graph
    """
    return str(v)

  def color(self, v):
    """
    Return a fillcolor to be used when displaying graph
    """
    return "blue"

def essential(hexcode,n,m):
  i = int(hexcode, 16)
  binning = [sum ([ 1 if i & 2**k else 0 for k in range(j*m,(j+1)*m)]) for j in range(0,2**n)]
  if not 0 in binning:
    return False 
  if not m in binning:
    return False
  N = 2**n 
  for d in range(0,n):
    bit = 2**d 
    flag = False
    for x in range(0, N):
      if binning[x] != binning[x ^ bit]:
        flag = True
        break
    if not flag:
      return False
  return True

class dsgrnDatabase:
  def __init__(self, database_name):
    """
    Initialize a DSGRN database object
    """
    self.dbname = database_name
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

  def _repr_svg_(self):
    return graphviz.Source(self.network.graphviz())._repr_svg_()

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
          yield [self.reduced_parameter_index(row[0], gene_index) + row for row in rows]

    c2 = self.conn.cursor() # need a second cursor to keep the place in the iteration as updates to the table occur
    c2.execute("select * from Signatures")
    for values in valuegenerator(c2):
      c.executemany('INSERT INTO ' + gene + ' VALUES (?,?,?,?)', values )
    c2.close()

    c.execute('create index ' + gene + '1 on ' + gene + ' (ReducedParameterIndex, GeneParameterIndex)') # gene is sanitized
    self.conn.commit()
    return (N, M)

  def single_gene_query (self, gene, reduced_parameter_index ):
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
    * `graph.mgi` is a function which accepts a gpi and returns the associated Morse graph idnex
    * `graph.num_inputs` is the number of network edges which are inputs to the gene associated with the query
    * `graph.num_outputs`is the number of network edges which are outputs to the gene associated with the query
    * `graph.essential` is a boolean-valued function which determines if each vertex corresponds to an essential parameter node
   """
    # Sanitize "gene":
    if gene not in self.names:
      raise NameError(gene + " is not the name of a node in the network")
    c = self.conn.cursor()
    c.execute("select GeneParameterIndex,ParameterIndex,MorseGraphIndex from " + gene + " where ReducedParameterIndex=?" , (str(reduced_parameter_index),))
    gene_index = self.network.index(gene)
    factorgraph = self.parametergraph.factorgraph(gene_index)
    Q = { row[0] : (factorgraph[row[0]],  row[1], row[2]) for row in c }
    isPowerOfTwo = lambda n : (n & (n-1) == 0) and (n != 0)
    isAdjacentHexcode = lambda hexcode1, hexcode2 : int(hexcode1, 16) < int(hexcode2, 16) and isPowerOfTwo( int(hexcode1, 16) ^ int(hexcode2, 16) )
    vertices = set(Q.keys())
    edges = [ (gpi1, gpi2) for gpi1 in Q.keys() for gpi2 in Q.keys() if isAdjacentHexcode(Q[gpi1][0], Q[gpi2][0]) ]
    graph = Graph(vertices,edges)
    graph.data = Q
    graph.mgi = lambda gpi : graph.data[gpi][2]
    graph.label = lambda gpi : str(graph.data[gpi][1]) + ':' + str(graph.data[gpi][2])
    graph.num_inputs = len(self.network.inputs(gene_index))
    graph.num_outputs = len(self.network.outputs(gene_index))
    graph.essential = lambda gpi : essential(graph.data[gpi][0],graph.num_inputs,graph.num_outputs)
    return graph

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

  ## FP QUERY MECHANISMS
  # SingleFPQuery and DoubleFPQuery
  # (use FPString, buildQueryExpression, MatchQuery as helper methods)

  def FPString(self, i, j):
    terms = [ "_" for k in range(0, self.D) ]
    terms[i] = str(j)  
    expression = "Label like 'FP { " + ', '.join(terms) + "%'";
    return expression

  def buildQueryExpression(self, bounds):
    expressions = []
    for i in range(0,self.D):
      networknodename = self.names[i]
      lowerbound = 0
      upperbound = self.D
      if networknodename in bounds:
        varbounds = bounds[networknodename]
        if type(varbounds) == int :
          lowerbound = varbounds
          upperbound = varbounds
        else:
          lowerbound = varbounds[0]
          upperbound = varbounds[1]
      expression = " or ".join([ self.FPString(i,j) for j in range(lowerbound, upperbound+1)])
      expressions.append(expression)
    return expressions

  def MatchQuery(self, bounds,outputtablename):
    # Parse the command line
    c = self.conn.cursor()
    expressions = self.buildQueryExpression(bounds)
    N = len(expressions)
    for i, expression in enumerate(expressions):
      oldtable = 'tmpMatches' + str(i)
      if i == 0: oldtable = "MorseGraphAnnotations"
      newtable = 'tmpMatches' + str(i+1)
      if i == N-1: newtable = outputtablename
      c.execute('create temp table ' + newtable + ' as select * from ' + oldtable + ' where ' + expression + ';')
      if i > 0: c.execute('drop table ' + oldtable);

  def SingleFPQuery (self, bounds):
    self.MatchQuery(bounds,"Matches")
    # Final query and print results
    c = self.conn.cursor()
    result = [ row[0] for row in c.execute('select MorseGraphIndex from Matches;')]
    #result = [row[0] for row in c.execute('select distinct ParameterIndex from Matches natural join Signatures order by ParameterIndex;')]
    c.execute('drop table Matches');
    return result

  def DoubleFPQuery (self, bounds1, bounds2):
    self.MatchQuery(bounds1,"Matches1");
    self.MatchQuery(bounds2,"Matches2");
    c = self.conn.cursor()
    c.execute('create temp table Left as select MorseGraphIndex ' +
              'from (select * from Matches1 except select * from Matches2) group by MorseGraphIndex;')
    c.execute('create temp table Middle as select MorseGraphIndex, count(Vertex) as middle ' +
              'from (select * from Matches1 intersect select * from Matches2) group by MorseGraphIndex;')
    c.execute('create temp table Right as select MorseGraphIndex ' +
              'from (select * from Matches2 except select * from Matches1) group by MorseGraphIndex;')
    c.execute('create temp table Matches as ' +
              'select * from (select * from Left intersect select * from Right) ' +
              'union ' +
              'select * from (select MorseGraphIndex from Middle ' +
                             'intersect ' +
                             'select * from (select * from Left union select * from Right)) ' +
              'union ' +
              'select MorseGraphIndex from Middle where middle >= 2;')

    # Final query and print results
    result = [ row[0] for row in c.execute('select MorseGraphIndex from Matches;')]
    #result = [ row[0] for row in c.execute('select distinct ParameterIndex from Matches natural join Signatures order by ParameterIndex;')]
    c.execute('drop table Matches1');
    c.execute('drop table Matches2');
    c.execute('drop table Matches');
    c.execute('drop table Left');
    c.execute('drop table Middle');
    c.execute('drop table Right');
    return result

  def MonostableFPQuery (self, bounds):
    return set(self.SingleFPQuery(bounds)).intersection(self.uniquestablemorsegraphindices)

  def full_inducibility (self, gene, bounds1, bounds2, max_gpi):
    #Assume database is calculated and single gene prepare is done
    Matches1 = frozenset(self.MonostableFPQuery(bounds1))
    Matches2 = frozenset(self.MonostableFPQuery(bounds2))
    # MatchesDouble = frozenset(self.DoubleFPQuery(bounds1,bounds2).intersection(self.bistablemorsegraphindices)) 
    #strict bistability not required
    MatchesDouble = frozenset(self.DoubleFPQuery(bounds1,bounds2)) ##This includes bistability at lowest and highest nodes
    c = self.conn.cursor()
    c.execute('create index ' + gene + '2 on ' + gene + ' (GeneParameterIndex)')

    FP1_reduced_params = set([])
    for row in c.execute("select MorseGraphIndex,ReducedParameterIndex from " + gene + " where GeneParameterIndex=0;"):
      if row[0] in Matches1: FP1_reduced_params.add(row[1])

    FP2_reduced_params = set([])
    for row in c.execute("select MorseGraphIndex,ReducedParameterIndex from " + gene + " where GeneParameterIndex=" + str(max_gpi) + ";"):
      if row[0] in Matches2: FP2_reduced_params.add(row[1])

    Bistab_reduced_params = set([])
    for row in c.execute("select MorseGraphIndex,ReducedParameterIndex from " + gene + " where GeneParameterIndex!=0 and GeneParameterIndex!=" + str(max_gpi) + ";"):
      if row[0] in MatchesDouble: Bistab_reduced_params.add(row[1])

    return FP1_reduced_params, FP2_reduced_params, Bistab_reduced_params 