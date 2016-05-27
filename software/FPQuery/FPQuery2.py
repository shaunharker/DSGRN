#!/usr/bin/python
# FPQuery2
# Usage:
#
#  This command takes one of five keywords as its first argument, and depending on the keyword possibly extra arguments
#  Details follow
#
#   First Usage:
#
#   python ./FPQuery2.py database_filename SingleFP JSON-STRING-SPECIFYING-ALLOWED-FP-LOCATIONS
#
#   Return list of parameters for which there exists an FP node in the specified region
#
#   Example:
#     python ./FPQuery2.py 5D_2016_02_08_cancer_withRP_essential_VA.db SingleFP '{"EE1":[2,4],"RP":[0,4]}' > query.txt
#
#    This means variable EE1 can be in bin 2, 3, or 4, while variable RP can be in bins 0, 1, 2, 3, or 4. All
#    other variables can be in any bin
#
#   Second Usage:
# 
#   python ./FPQuery2.py database_filename DoubleFP JSON1 JSON2
#   
#   Return list of parameters for which there exists an FP node in the region indicated by JSON1
#   and another (distinct from the first) FP node in the region indicated by JSON2
#   The JSON strings are in the same formet as SingleFP
#
#   Example:
#     python ./FPQuery2.py 5D_2016_02_08_cancer_withRP_essential_VA.db DoubleFP '{"EE1":[2,4],"RP":[0,4]}' '{"EE1":[2,4],"RP":[0,1]}'
#
#   Other usages:
#   python ./FPQuery2.py database_filename UniqueStable
#    returns list of parameters with a unique minimal Morse node
#
#   python ./FPQuery2.py database_filename Bistable
#     returns list of parameters with precisely two minimal Morse nodes
#
#   python ./FPQuery2.py database_filename Multistable
#     returns list of parameters with two or more minimal Morse nodes
#   
#  Note: In all cases, the parameters are returned in sorted order. In order to combine queries,
#        one solution is to use the "comm" command in the Unix bash shell, i.e.
#        comm -12 query1.txt query2.txt > intersectedquery.txt


import sys, json, sqlite3

# Connect to SQLite database
conn = sqlite3.connect(sys.argv[1])
c = conn.cursor()

# Retrieve Network Specification
c.execute('select Specification from Network;');
netspec = c.fetchone()[0]

# Index the variable of the network specification
variables = []
for line in netspec.split('\n'):
  var = line.partition(' ')[0]
  if len(var) > 0:
    variables.append(var)

D = len(variables)

def FPString(i, j):
  terms = [ "_" for k in range(0, D) ]
  terms[i] = str(j)  
  expression = "Label like 'FP { " + ', '.join(terms) + "%'";
  return expression

def buildQueryExpression(variables, bounds):
  expressions = []
  for i in range(0,D):
    networknodename = variables[i]
    lowerbound = 0
    upperbound = D
    if networknodename in bounds:
      varbounds = bounds[networknodename]
      if type(varbounds) == int :
        lowerbound = varbounds
        upperbound = varbounds
      else:
        lowerbound = varbounds[0]
        upperbound = varbounds[1]
    expression = " or ".join([ FPString(i,j) for j in range(lowerbound, upperbound+1)])
    expressions.append(expression)
  return expressions

def MatchQuery(variables,bounds,outputtablename):
  # Parse the command line
  expressions = buildQueryExpression(variables, bounds)
  N = len(expressions)
  for i, expression in enumerate(expressions):
    oldtable = 'tmpMatches' + str(i)
    if i == 0: oldtable = "MorseGraphAnnotations"
    newtable = 'tmpMatches' + str(i+1)
    if i == N-1: newtable = outputtablename
    c.execute('create temp table ' + newtable + ' as select * from ' + oldtable + ' where ' + expression + ';')
    if i > 0: c.execute('drop table ' + oldtable);

def SingleFPQuery (variables, bounds):
  MatchQuery(variables,bounds,"Matches")
  # Final query and print results
  for row in c.execute('select distinct ParameterIndex from Matches natural join Signatures order by ParameterIndex;'):
    print row[0]

def DoubleFPQuery (variables, bounds1, bounds2):
  MatchQuery(variables,bounds1,"Matches1");
  MatchQuery(variables,bounds2,"Matches2");
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
  for row in c.execute('select distinct ParameterIndex from Matches natural join Signatures order by ParameterIndex;'):
    print row[0]

command = sys.argv[2]

if command == "SingleFP":
  bounds = json.loads(sys.argv[3])
  print(bounds)
  SingleFPQuery(variables, bounds)

if command == "DoubleFP":
  bounds1 = json.loads(sys.argv[3])
  bounds2 = json.loads(sys.argv[4])
  DoubleFPQuery(variables,bounds1,bounds2)

if command == "UniqueStable":
  sqlexpression = "select ParameterIndex from Signatures natural join (select MorseGraphIndex, count(*) as StableCount from (select MorseGraphIndex,Vertex from MorseGraphVertices except select MorseGraphIndex,Source from MorseGraphEdges) group by MorseGraphIndex) where StableCount=1 order by ParameterIndex;"
  for row in c.execute(sqlexpression):
    print row[0]

if command == "Bistable":
  sqlexpression = "select ParameterIndex from Signatures natural join (select MorseGraphIndex, count(*) as StableCount from (select MorseGraphIndex,Vertex from MorseGraphVertices except select MorseGraphIndex,Source from MorseGraphEdges) group by MorseGraphIndex) where StableCount=2 order by ParameterIndex;"
  for row in c.execute(sqlexpression):
    print row[0]

if command == "Multistable":
  sqlexpression = "select ParameterIndex from Signatures natural join (select MorseGraphIndex, count(*) as StableCount from (select MorseGraphIndex,Vertex from MorseGraphVertices except select MorseGraphIndex,Source from MorseGraphEdges) group by MorseGraphIndex) where StableCount>1 order by ParameterIndex;"
  for row in c.execute(sqlexpression):
    print row[0]

# Close the SQL connection
conn.close();
