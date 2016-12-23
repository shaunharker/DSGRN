# PhenotypeQuery.py
# MIT LICENSE 2016
# Shaun Harker

class PhenotypeQuery:
  def __init__ (self, database, mgi):
    self.database = database
    c = database.conn.cursor()
    self.set_of_matches = set([ row[0] for row in c.execute('select ParameterIndex from Signatures where MorseGraphIndex=' + str(mgi) + ';')])

  def matches(self):
    """
    Return entire set of matches
    """
    return self.set_of_matches

  def __call__ (self, pi ):
    """ 
    Test if a single mgi is in the set of matches
    """
    return pi in self.set_of_matches

