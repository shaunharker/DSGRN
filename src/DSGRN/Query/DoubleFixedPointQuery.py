# DoubleFixedPointQuery.py
# MIT LICENSE 2016
# Shaun Harker

from DSGRN.Query.Logging import LogToSTDOUT
from DSGRN.Query.FixedPointTables import *

class DoubleFixedPointQuery:
  def __init__ (self, database, bounds1, bounds2):
    self.database = database
    MatchQuery(bounds1,"Matches1",database);
    MatchQuery(bounds2,"Matches2",database);
    c = database.conn.cursor()
    c.execute('create temp table Left as select MorseGraphIndex ' +
              'from (select * from Matches1 except select * from Matches2) group by MorseGraphIndex;')
    c.execute('create temp table Middle as select MorseGraphIndex, count(Vertex) as middle ' +
              'from (select * from Matches1 intersect select * from Matches2) group by MorseGraphIndex;')
    c.execute('create temp table Right as select MorseGraphIndex ' +
              'from (select * from Matches2 except select * from Matches1) group by MorseGraphIndex;')
    c.execute('create temp table Matches as ' +
              'select * from (select * from Left intersect select * from Right) ' +
              'union ' +
              'select * from (select MorseGraphIndex from Middle ' +
                             'intersect ' +
                             'select * from (select * from Left union select * from Right)) ' +
              'union ' +
              'select MorseGraphIndex from Middle where middle >= 2;')

    # Final query and print results
    self.set_of_matches = set([ row[0] for row in c.execute('select MorseGraphIndex from Matches;')])
    #result = [ row[0] for row in c.execute('select distinct ParameterIndex from Matches natural join Signatures order by ParameterIndex;')]
    c.execute('drop table Matches1');
    c.execute('drop table Matches2');
    c.execute('drop table Matches');
    c.execute('drop table Left');
    c.execute('drop table Middle');
    c.execute('drop table Right');

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
