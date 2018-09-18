# NstableQuery.py
# Bree Cummins
# Edited from
# MultistableQuery.py
# MIT LICENSE 2016
# Shaun Harker

class NstableQuery:
  """
  NstableQuery
    Query to check if morse graph index corresponds to a Morse graph with at least N minimal fixed points
  """
  def __init__ (self, database, N ):
    self.database = database
    c = database.conn.cursor()
    sqlexpression = "select MorseGraphIndex from (select MorseGraphIndex, count(*) as StableCount from (select MorseGraphIndex,Vertex from MorseGraphAnnotations where Label like 'FP%' except select MorseGraphIndex,Source from MorseGraphEdges) group by MorseGraphIndex) where StableCount>{};".format(N-1)
    database.NQuery = frozenset([ row[0] for row in c.execute(sqlexpression) ])

  def matches(self):
    """
    Return entire set of matches
    """
    return self.database.NQuery

  def __call__ (self, morsegraphindex ):
    """ 
    Test if a single mgi is in the set of matches
    """
    return morsegraphindex in self.database.NQuery