# MultistableQuery.py
# MIT LICENSE 2016
# Shaun Harker

class MultistableQuery:
  """
  MultistableQuery
    Query to check if morse graph index corresponds to a Morse graph with more than one minimal element
  """
  def __init__ (self, database ):
    self.database = database
    if not hasattr(database, 'MultistableQuery'):
      c = database.conn.cursor()
      sqlexpression = "select MorseGraphIndex from (select MorseGraphIndex, count(*) as StableCount from (select MorseGraphIndex,Vertex from MorseGraphVertices except select MorseGraphIndex,Source from MorseGraphEdges) group by MorseGraphIndex) where StableCount>1;"
      database.MultistableQuery = frozenset([ row[0] for row in c.execute(sqlexpression) ])

  def matches(self):
    """
    Return entire set of matches
    """
    return self.database.MultistableQuery

  def __call__ (self, morsegraphindex ):
    """ 
    Test if a single mgi is in the set of matches
    """
    return morsegraphindex in self.database.MultistableQuery
