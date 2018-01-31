# SingleFixedPointQuery.py
# MIT LICENSE 2016
# Shaun Harker

from DSGRN.Query.FixedPointTables import *
from DSGRN.Query.Logging import LogToSTDOUT

class SingleFixedPointQuery:
  def __init__ (self, database, bounds):
    self.database = database
    LogToSTDOUT("SingleFixedPointQuery :: initializing")
    LogToSTDOUT("SingleFixedPointQuery :: calling MatchQuery")
    MatchQuery(bounds,"Matches",database)
    # Final query and print results
    c = database.conn.cursor()
    LogToSTDOUT("SingleFixedPointQuery :: select MorseGraphIndex from Matches;")
    self.set_of_matches = set([ row[0] for row in c.execute('select MorseGraphIndex from Matches;')])
    #result = [row[0] for row in c.execute('select distinct ParameterIndex from Matches natural join Signatures order by ParameterIndex;')]
    LogToSTDOUT("SingleFixedPointQuery :: drop table Matches;")
    c.execute('drop table Matches');
    LogToSTDOUT("SingleFixedPointQuery :: constructed")


  def matches(self):
    """
    Return entire set of matches
    """
    return self.set_of_matches

  def __call__ (self, morsegraphindex ):
    """ 
    Test if a single mgi is in the set of matches
    """
    return morsegraphindex in self.set_of_matches

