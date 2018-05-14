import sqlite3
import graphviz
from DSGRN._dsgrn import *
from functools import reduce

from DSGRN.Query.Logging import LogToSTDOUT

class Database:
  def __init__(self, database_name):
    """
    Initialize a DSGRN database object
    """
    self.dbname = database_name
    self.conn = sqlite3.connect(database_name)
    self.cursor = self.conn.cursor()
    # Load network spec from database
    sqlexpression = "select Specification from Network"
    self.cursor.execute(sqlexpression)
    network_spec = self.cursor.fetchone()[0]
    # construct network
    self.network = Network(network_spec)
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
    self.indexing_place_values = reduce ( lambda x, y : x + [x[-1]*y], self.indexing_place_bases[:-1], [1])

  def execute(self, expression, parameters = None):
    """
    Perform an SQL query.
    Returns a "cursor" object (see python sqlite3 API for details)
    """
    return self.cursor.execute(expression, parameters)

  def __call__(self, pi):
    c = self.conn.cursor()
    sqlexpression = "select MorseGraphIndex from Signatures where ParameterIndex = ?"
    c.execute(sqlexpression,(pi,))
    mgi = c.fetchone()[0]
    return mgi

  def __del__(self):
    """
    Commit and close upon destruction
    """
    self.conn.commit()
    self.conn.close()

  def _repr_svg_(self):
    return graphviz.Source(self.network.graphviz())._repr_svg_()

  def DrawMorseGraph(self, morsegraphindex):
    """
    Return an object which renders to a graphviz representation in Jupyter
    """
    c = self.conn.cursor()
    sqlexpression = "select Graphviz from MorseGraphViz where MorseGraphIndex = ?"
    c.execute(sqlexpression,(morsegraphindex,))
    gv = c.fetchone()[0]
    return graphviz.Source(gv)
