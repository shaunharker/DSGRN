# HysteresisQuery.py
# MIT LICENSE 2016
# Shaun Harker

def AlignmentGraph ( graph1, graph2, matching_relation ):
  """
  Creates alignment graph of graph1 and graph2 (i.e. induced subgraph of tensor product where "matching_label" attribute matches)
  The graph structures must have: 
    an attribute "vertices" which is the set of vertices,
    a method "adjacency" which gives adjacency lists,
    an attribute "matching_label"
  """
  vertices = set([ (u, v) for u in graph1.vertices for v in graph2.vertices if matching_relation(graph1.matching_label(u),graph2.matching_label(v)) ])
  edges = [ ((x,y),(u,v)) for (x,y) in vertices for u in graph1.adjacencies(x) for v in graph2.adjacencies(y) if (u,v) in vertices ]
  return Graph(vertices, edges)

class HysteresisQuery:
  def __init__(self, database, gene, quiescent_bounds, proliferative_bounds):
    """
    In order to perform hysteresis queries we must first categorize each Morse graph as either 
    'Q' quiescent, 'P' proliferative, 'B' bistable, or 'O' other
    We assume the quiescent and proliferative FP states are given by disjoint bounding rectangles
    """
    self.database = database
    self.gene = gene
    # Create query object to check if morse graph indices have quiescent FP
    Q = SingleFPQuery(database, quiescent_bounds)
    # Create query object to check if morse graph indices have proliferative FP
    P = SingleFPQuery(database, proliferative_bounds)
    # Create a labelling function which accepts a morse graph index and returns Q, P, B, or O
    self.matching_label = lambda mgi : { (False,False):'O', (False,True):'P', (True,False):'Q', (True,True) : 'B'}[(Q(mgi), P(mgi))]
    # Create the pattern graph to represent Q -> B -> P (with self-loop on Q, B, and P)
    self.patterngraph = Graph(set([0,1,2]), [(0,0),(0,1),(1,1),(1,2),(2,2)])
    self.patterngraph.matching_label = { 0:'Q', 1:'B', 2:'P' }
    # Create matching relation (in this case we just check for equality of the matching labels)
    self.matching_relation = lambda label1, label2 : label1 == label2
    # Create SingleGeneQuery object
    self.single_gene_query = SingleGeneQuery(database, gene)

  def __call__(self, gene, reduced_parameter_index):
    searchgraph = self.single_gene_query(reduced_parameter_index)
    searchgraph.matching_label = { v : self.matching_label(searchgraph.mgi(v)) for v in searchgraph.vertices }
    alignment_graph = AlignmentGraph(searchgraph, self.patterngraph, self.matching_relation)
    root_vertex = (0,0)
    leaf_vertex = (len(searchgraph.vertices)-1, 2)
    return alignment_graph.number_of_paths(root_vertex, leaf_vertex) > 0
    
