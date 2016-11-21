
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

  def adjacencies(self, p):
    """
    Return adjacency list for vertex p (i.e. { q : (p,q) \in edges })
    """
    return self.adjacency_lists.get(p,[])

  def numberOfPaths(self, p, q):
    """
    Compute the number of paths from "p" to "q" in the graph described by "adjacency_lists"
    """
    cache = {q : 1}
    subresult = lambda x : cache.get(x, sum([subresult(y) for y in self.adjacencies(x)]))
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