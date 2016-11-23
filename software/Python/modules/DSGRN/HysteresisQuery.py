# HysteresisQuery.py
# MIT LICENSE 2016
# Shaun Harker

from Graph import *
from AlignmentGraph import *
from MonostableFixedPointQuery import *
from DoubleFixedPointQuery import *
from SingleGeneQuery import *

class HysteresisQuery:
  """
  HysteresisQuery
    A HysteresisQuery post-processes the result graph of a SingleGeneQuery
    and returns true if there exists a path from the root of the graph 
    to the leaf of the graph which has the following property:
      The path matches the regular expression 
        Q(Q|q)*BB*(P|p)*P, i.e. starts at Q at root, passes through Q or q zero or more times,
        then B one or more times, and then p or P zero or more times and ends up at P at leaf
      where vertices are labeled
        Q if it matches MonostableFixedPoint(quiescent_bounds) query
        P if it matches MonostableFixedPoint(proliferative_bounds) query
        B if it matches DoubleFixedPointQuery(database, quiescent_bounds, proliferative_bounds)
        O otherwise
  """
  def __init__(self, database, gene, quiescent_bounds, proliferative_bounds):
    """
    In order to perform hysteresis queries we must first categorize each Morse graph as either 
    'Q' quiescent, 'P' proliferative, 'B' bistable, or 'O' other
    We assume the quiescent and proliferative FP states are given by disjoint bounding rectangles
    """
    self.database = database
    self.gene = gene
    # Create query object to check if morse graph indices have quiescent FP as only minimal morse node
    Q = MonostableFixedPointQuery(database, quiescent_bounds)
    # Create query object to check if morse graph indices has quiescent FP
    q = SingleFixedPointQuery(database, quiescent_bounds)
    # Create query object to check if morse graph indices have proliferative FP as only minimal morse node
    P = MonostableFixedPointQuery(database, proliferative_bounds)
    # Create query object to check if morse graph indices has proliferative FP
    p = SingleFixedPointQuery(database, proliferative_bounds)
    # Check query object to check if morse graph index has both quiescent FP and proliferative FP
    B = DoubleFixedPointQuery(database, quiescent_bounds, proliferative_bounds)
    # Create a labelling function which accepts a morse graph index and returns Q, P, B, p, q, or O
    # Note: case fallthrough order matters here
    self.matching_label = lambda mgi : 'Q' if Q(mgi) else ( 'P' if P(mgi) else ( 'B' if B(mgi) else ( 'q' if q(mgi) else ( 'p' if p(mgi) else 'O') ) ) )
    # Create the pattern graph to represent Q -> B -> P (with self-loop on Q, B, and P)
    self.patterngraph = Graph(set([0,1,2,3,4]), [(0,0),(1,1),(0,1),(1,0),(0,2),(1,2),(2,2),(2,3),(2,4)(3,3),(3,4),(4,4),(4,3)])
    self.patterngraph.matching_label = lambda v : { 0:'Q', 1:'q', 2:'B', 3:'p', 4:'P' }[v]
    # Create matching relation (in this case we just check for equality of the matching labels)
    self.matching_relation = lambda label1, label2 : label1 == label2
    # Create SingleGeneQuery object
    self.GeneQuery = SingleGeneQuery(database, gene)

  def __call__(self, reduced_parameter_index):
    searchgraph = self.GeneQuery(reduced_parameter_index)
    searchgraph.matching_label = lambda v : self.matching_label(searchgraph.mgi(v))
    alignment_graph = AlignmentGraph(searchgraph, self.patterngraph, self.matching_relation)
    root_vertex = (0,0)
    leaf_vertex = (len(searchgraph.vertices)-1, 4)
    return alignment_graph.numberOfPaths(root_vertex, leaf_vertex) > 0
    
