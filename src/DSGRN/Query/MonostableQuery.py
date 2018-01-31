# MonostableQuery.py
# MIT LICENSE 2016
# Shaun Harker

from DSGRN.Query.Logging import LogToSTDOUT

class MonostableQuery:
  """
  MonostableQuery
    Query to check if morse graph index corresponds to a Morse graph has a unique minimal element
  """
  def __init__ ( self, database ):
    LogToSTDOUT("MonostableQuery :: initializing")
    self.database = database
    if not hasattr(database, 'MonostableQuery'):
      c = database.conn.cursor()
      LogToSTDOUT("MonostableQuery :: select MorseGraphIndex from (select MorseGraphIndex, count(*) as StableCount from (select MorseGraphIndex,Vertex from MorseGraphVertices except select MorseGraphIndex,Source from MorseGraphEdges) group by MorseGraphIndex) where StableCount=1;")
      sqlexpression = "select MorseGraphIndex from (select MorseGraphIndex, count(*) as StableCount from (select MorseGraphIndex,Vertex from MorseGraphVertices except select MorseGraphIndex,Source from MorseGraphEdges) group by MorseGraphIndex) where StableCount=1;"
      database.MonostableQuery = frozenset([ row[0] for row in c.execute(sqlexpression) ])
    LogToSTDOUT("MonostableQuery :: constructed")

  def matches(self):
    """
    Return entire set of matches
    """
    return self.database.MonostableQuery

  def __call__ ( self, morsegraphindex ):
    """ 
    Test if a single mgi is in the set of matches
    """
    return morsegraphindex in self.database.MonostableQuery
