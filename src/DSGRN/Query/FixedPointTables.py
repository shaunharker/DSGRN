# FixedPointTables.py
# MIT LICENSE 2016
# Shaun Harker

# This file contains routines used by 
#   SingleFixedPointQuery.py  and
#   DoubleFixedPointQuery.py
# in order to create SQL tables to support their queries.

from DSGRN.Query.Logging import LogToSTDOUT

def FPString(i, j, database):
  terms = [ "_" for k in range(0, database.D) ]
  terms[i] = str(j)  
  expression = "Label like 'FP { " + ', '.join(terms) + "%'";
  return expression

def buildQueryExpression(bounds, database):
  expressions = []
  for i in range(0,database.D):
    networknodename = database.names[i]
    lowerbound = 0
    upperbound = len(database.network.outputs(i))
    if networknodename in bounds:
      varbounds = bounds[networknodename]
      if type(varbounds) == int :
        lowerbound = varbounds
        upperbound = varbounds
      else:
        lowerbound = varbounds[0]
        upperbound = varbounds[1]
    expression = " or ".join([ FPString(i,j,database) for j in range(lowerbound, upperbound+1)])
    expressions.append(expression)
  return expressions

def MatchQuery(bounds, outputtablename, database):
  # Parse the command line
  LogToSTDOUT("MatchQuery(" + str(bounds) + ", " + str(outputtablename) + ")")
  c = database.conn.cursor()
  expressions = buildQueryExpression(bounds, database)
  LogToSTDOUT("MatchQuery :: built expressions " + str(expressions) )
  N = len(expressions)
  for i, expression in enumerate(expressions):
    oldtable = 'tmpMatches' + str(i)
    if i == 0: oldtable = "MorseGraphAnnotations"
    newtable = 'tmpMatches' + str(i+1)
    if i == N-1: newtable = outputtablename
    sql_string = 'create temp table ' + newtable + ' as select * from ' + oldtable + ' where ' + expression + ';'
    LogToSTDOUT("MatchQuery :: " + sql_string)
    c.execute(sql_string)
    if i > 0: c.execute('drop table ' + oldtable);
  LogToSTDOUT("MatchQuery :: constructed")



