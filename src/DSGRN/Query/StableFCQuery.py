# StableFCQuery.py
# MIT LICENSE 2016
# Shaun Harker

from collections import defaultdict

class StableFCQuery:
  def __init__ (self, database):
    self.database = database
    c = database.conn.cursor()
    query_result = [ row for row in c.execute('select ParameterIndex, Vertex from Signatures natural join (select MorseGraphIndex,Vertex from (select MorseGraphIndex,Vertex from MorseGraphAnnotations where Label="FC" except select MorseGraphIndex,Source from MorseGraphEdges));')]
    self.query_data = defaultdict(set)
    for parameter_index, morse_node in query_result:
      self.query_data[parameter_index].add(morse_node)
    #result = [row[0] for row in c.execute('select distinct ParameterIndex from Matches natural join Signatures order by ParameterIndex;')]

  def matches(self):
    """
    Return entire set of matches
    """
    return self.query_data

  def __call__ (self, parameter_index ):
    """ 
    Test if a single parameter index is in the set of matches
    """
    return self.query_data[parameter_index]
