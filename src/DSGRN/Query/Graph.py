
import graphviz

from DSGRN._dsgrn import *

def DrawGraph(g):
  """
  Return an object which renders in Notebook visualizations.
  Works with any input g with a "graphviz" method returning a valid
  graphviz string
  """
  return graphviz.Source(g.graphviz())

def DrawGraphWithHighlightedPath(g, path):
  """
  Return an object which renders in Notebook visualizations.
  Works for "SearchGraph" "PatternGraph" and "MatchingGraph".
  If given a python integer list it can convert for "SearchGraph" and "PatternGraph"
  Currently "MatchingGraph" requires an IntPairList input
  """
  return graphviz.Source(g.graphviz_with_highlighted_path(path))

class Graph:
  def __init__(self, vertices, edges):
    """
    Construct graph from set of vertices and collection of edges
    Note: vertices and edges can be presented as either lists of sets;
          they will be interally converted to sets.
    """
    self.vertices = set(vertices)
    self.edges = set(edges) 
    self.adjacency_lists = {}
    for s,t in self.edges:
      if not s in self.vertices:
        self.vertices.add(s)
      if not t in self.vertices:
        self.vertices.add(t)
      if not s in self.adjacency_lists:
          self.adjacency_lists[s] = []
      self.adjacency_lists[s].append(t) 
    self.vertexname = {}
    for i, v in enumerate(self.vertices):
      self.vertexname[v] = 'X' + str(i)

  def clone(self):
    """
    Return a deep copy
    """
    return Graph(self.vertices, self.edges)

  def _repr_svg_(self):
    """
    Return svg representation suitable for Notebook visualization
    """
    return graphviz.Source(self.graphviz())._repr_svg_()

  def graphviz(self):
    """
    Return graphviz string for graph
    """
    return 'digraph {' + \
  '\n'.join([ '"' + self.vertexname[v] + '" [label="' + self.label(v) + '";style="filled";fillcolor="' + self.color(v) + '"];' for v in self.vertices ]) + \
   '\n' + '\n'.join([ '"' + self.vertexname[u]  + '" -> "' + self.vertexname[v]  + '";' for (u, v) in self.edges ]) + \
   '\n' + '}\n'

  def adjacencies(self, p):
    """
    Return adjacency list for vertex p (i.e. { q : (p,q) \in edges })
    """
    return self.adjacency_lists.get(p,[])

  def reachable(self, p, q):
    """
    Return "true" if there is a path from p to q
    """
    explored = set()
    items = [p]
    while items:
      x = items.pop()
      if x == q: return True
      if x in explored: continue
      explored.add(x)
      items.extend([ y for y in self.adjacencies(x) if y not in explored ])
    return False

  def predicate_reachable(self, p, allowed, terminal):
    """
    Return "true" if there is a path from the vertex p to some vertex q such that
    each node in the path (except possibly q) satisfies allowed(p) and q satisfies terminal(q)
    """
    if not allowed(p):
      return False
    explored = set()
    items = [p]
    while items:
      x = items.pop()
      if terminal(x): return True
      if x in explored: continue
      if not allowed(x): continue
      explored.add(x)
      items.extend([ y for y in self.adjacencies(x) if y not in explored ])
    return False

  def numberOfPaths(self, p, q):
    """
    Compute the number of paths from "p" to "q" in the graph described by "adjacency_lists"
    TODO: this appears to be unexpectedly slow
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
