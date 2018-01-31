# AlignmentGraph.py
# MIT LICENSE 2016
# Shaun Harker

from DSGRN.Query.Graph import *

def AlignmentGraph ( graph1, graph2, matching_relation ):
  """
  Creates alignment graph of graph1 and graph2 (i.e. induced subgraph of tensor product where "matching_label" attribute matches)
  The graph structures must have: 
    an attribute "vertices" which is the set of vertices,
    a method "adjacency" which gives adjacency lists,
    an attribute "matching_label" so "matching_label(v)" gives a label for each vertex
  """
  vertices = set([ (u, v) for u in graph1.vertices for v in graph2.vertices if matching_relation(graph1.matching_label(u),graph2.matching_label(v)) ])
  edges = [ ((x,y),(u,v)) for (x,y) in vertices for u in graph1.adjacencies(x) for v in graph2.adjacencies(y) if (u,v) in vertices ]
  return Graph(vertices, edges)
