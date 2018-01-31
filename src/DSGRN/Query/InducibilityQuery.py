# InducibilityQuery.py
# MIT LICENSE 2016
# Shaun Harker and Bree Cummins

from DSGRN.Query.SingleFixedPointQuery import *
from DSGRN.Query.DoubleFixedPointQuery import *
from DSGRN.Query.MonostableFixedPointQuery import *
from DSGRN.Query.SingleGeneQuery import *

class InducibilityQuery:
  """
  InducibilityQuery. 
    The query depends on a choice of gene, and two sets of domain bounds named bounds1 and bounds2.
    Given: a reduced parameter index 
    Return: a triple of boolean values (a,b,c) such that:
      a is true if and only if (0,reduced_parameter_index) is monostable and has an FP in bounds1
      b is true if and only if (max_gpi,reduced_parameter_index) is monostable and has an FP in bounds2
      c is true if and only if (gpi,reduced_parameter_index) has an FP in bounds1 and an FP in bounds2
                               for some 0 < gpi < max_gpi
  """
  def __init__(self, database, gene, bounds1, bounds2):
    self.QueryFP1 = MonostableFixedPointQuery(database, bounds1)
    self.QueryFP2 = MonostableFixedPointQuery(database, bounds2)
    self.QueryDoubleFP = DoubleFixedPointQuery(database,bounds1,bounds2)
    self.GeneQuery = SingleGeneQuery(database, gene)
    self.max_gpi = self.GeneQuery.number_of_gene_parameters() - 1

  def __call__(self, reduced_parameter_index):
    """
    Given a reduced parameter index, return a triple of boolean values (a,b,c)
    """
    query_graph = self.GeneQuery(reduced_parameter_index)
    a = self.QueryFP1(query_graph.mgi(0))
    b = self.QueryFP2(query_graph.mgi(self.max_gpi))
    c = any ( self.QueryDoubleFP(query_graph.mgi(i)) for i in range(1,self.max_gpi) )
    return (a, b, c)