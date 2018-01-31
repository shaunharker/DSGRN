# MonostableFixedPointQuery.py
# MIT LICENSE 2016
# Shaun Harker

from DSGRN.Query.SingleFixedPointQuery import *
from DSGRN.Query.MonostableQuery import *
from DSGRN.Query.Logging import LogToSTDOUT

class MonostableFixedPointQuery:
  """
  MonostableFixedPointQuery
    Enables queries to determine if a Morse Graph Index 
    corresponds to a Morse Graph for which there is
    (a) a single minimal Morse node
    (b) the Morse node is annotated as an FP within specified bounds
  """
  def __init__(self, database, bounds):
    LogToSTDOUT("MonostableFixedPointQuery :: initializing")
    self.set_of_matches = SingleFixedPointQuery(database,bounds).matches() & MonostableQuery(database).matches()
    LogToSTDOUT("MonostableFixedPointQuery :: constructed")


  def matches(self):
    """
    Return entire set of matches
    """
    return self.set_of_matches

  def __call__(self, mgi):
    """ 
    Test if a single mgi is in the set of matches
    """
    return mgi in self.set_of_matches