# The MIT License (MIT)

# Copyright (c) 2016 Breschine Cummins

# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:

# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.

# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.

import numpy as np
from scipy.integrate import ode
import matplotlib.pyplot as plt
"""
import matplotlib
font = {'family' : 'normal',
        'size'   : 22}
matplotlib.rc('font', **font)
"""


class hillmodel(object):
  '''
  This class takes a network file, a parameter sample file, and a Hill
  exponent and builds a Hill function model. The class has two public
  methods:
  1) time,timeseries = hillmodel.simulateHillModel(initialconditions,initialtime,finaltime,timestep)
  2) hillmodel.plotResults(times,timeseries)
  The first method generates a time series for a given set of initial conditions,
  and the second method plots the results. 
  '''
  def __init__(self,networkfile,samplefile,hillexp):
    '''
    Construct the Hill model for a given network and parameter sample.
    '''
    eqnstr,self.varnames = self._parseEqns(networkfile)
    parameternames,samples = self._parseSamples(self.varnames,samplefile)
    self.eqns=self._makeHillEqns(eqnstr,parameternames,samples,hillexp)

  def simulateHillModel(self,initialconditions,initialtime,finaltime,timestep):
    '''
    Simulate the constructed Hill model for a given set of initial conditions 
    and time period. The given time step only specifies which output timeseries
    is returned. The time step for the backwards difference ODE solver is 
    determined by the algorithm.

    '''
    def RHS(t,x,eqns):
      return np.array(eqns(x))
    def integrate(r,y0,t0,t1,dt):
      times=[t0]
      timeseries=[y0]
      while r.successful() and r.t < t1:
        r.integrate(r.t+dt)
        times.append(r.t)
        timeseries.append(r.y)
      return times,timeseries
    r = ode(RHS).set_integrator('vode', method='bdf')
    r.set_initial_value(initialconditions,initialtime).set_f_params(self.eqns)
    times,timeseries = integrate(r,initialconditions,initialtime,finaltime,timestep)
    return times,timeseries

  def plotResults(self,times,timeseries,plotoptions={},legendoptions={},figuresize=()):
    '''
    Plot a time series.

    plotoptions and legendoptions are optional dictionaries with keys corresponding 
    to the options for matplotlib.pyplot.plot and matplotlib.pyplot.legend.

    Examples: 
    plotoptions={'linewidth':2}
    legendoptions={'fontsize':24,'loc':'upper left', 'bbox_to_anchor':(1, 1)}
    figuresize = (20,10)

    '''
    if figuresize:
      plt.figure(figsize=figuresize)
    timeseries=np.array(timeseries)
    for k in range(timeseries.shape[1]):
      plt.plot(times,timeseries[:,k],label=self.varnames[k],**plotoptions)
    plt.legend(**legendoptions)
    plt.axis('tight')
    plt.show()

# The remainder of the file consists of private methods implementing various parsing voodoo.

  def _parseEqns(self,fname='equations.txt'):
    """
    Parse a network specification file to obtain data structures representing ODEs
      Input: "fname" is a path to a network specification file.
      Output: The function outputs 
                eqnstr, varnames   
              where
                eqnstr   is a list of strings representing the ODE in a p-n formatting (see below)
                varnames is a dictionary with keys being the variable names and values being an internal indexing

      Note: "p-n formatting" of a network node's input formula replaces the variables occuring in 
            the string instead with the variable indices and suffixes them with either "n" or "p" 
            depending on whether they are negated. Multiplication in this formatting is always explicit 
            (never mere juxtaposition). It is easiest to describe by example:  
                      (~X + Y)(Z)  becomes ((0n)+(1p))*((2p)) when 
                      varnames["X"] == 0, varnames["Y"] == 1, and varnames["Z"] == 2
      Notes on Network specification file:
          A network spec file contains on each line 
            <varname> : <input-formula> [: E]
            (The optional last colon and what follows we may ignore.)
            An input-formula is an algebraic combination of variable names which allows the
            usage of the variables and the symbols (, ), +, and * in the usual ways. We may also
            prepend any variable name with the symbol "~". 
          We note the following:
            (a) Some variable names are contained inside of other variable names
            (b) There may be redundant whitespace (even between ~ and variable name)
            (c) We may write "X*Y" "X(Y)" "(X)Y" "X Y" which are equivalent and refer to the product, 
              but "XY" can only refer to a single variable "XY", and not the product of "X" and "Y".
    """
    file = open(fname,'r')
    eqns=[]
    variableToIndex = {}
    for line in file:
      # Parse line
      parsed = line.split(':')
      varname = parsed[0].strip() # e.g. "X"
      formula = parsed[1].strip() # e.g. "(~X + Y)U Z"
      # Disregard network spec comment lines
      continue if varname[0] == '.' or varname[0] == '@' 
      # Add entry in lookup table from variable name to variable index
      varnames[varname]=str(len(varnames))
      # Add entry in list of network node input formulas
      eqns.append(formula)
    file.close()
    eqnstr=[]
    for e in eqns:
      # Replace occurences of variables with variable indices followed by p if occurring with ~ prefix and followed by n otherwise
      # Example: "(~X + Y)U Z" --> "((0n) + (1p))(2p) (3p)"
      e = re.sub('([ ()+*]*)(~?) *([^ ~()+*]+)([ ()+*]*)', lambda x: x.group(1) + "(" + varnames[x.group(3)] + ("n" if (x.group(2) == '~') else "p") + ')' + x.group(4), e)
      # Remove spaces and make multiplications explicit
      # Example: "((0n) + (1p))(2p) (3p)" --> "((0n)+(1p))*(2p)*(3p)"
      e = e.replace(' ','').replace(')(',')*(')
      # Add parsed equation to eqnstr output list
      eqnstr.append(e)
    return eqnstr,varnames
      
  def _parseParameter(self,varnames,parameter):
    """
    Converts a parameter for internal use by replacing variable names with indices in the lookup table
    Inputs:  "varnames" is a dictionary with keys being the variable names and values being an internal indexing
                e.g. { "X" : "0", "Y" : "1", ... }
             "parameter" is a key-value table from parameter names to numeric values, e.g.
                e.g. { "L[X, Y]" : 2.34848, "U[X, Y]" : 1.23888, ... }
    Outputs: the return value is obtained from rewriting parameter names using internal variable indexing
                e.g. { "L[0,1]" : 2.34848, "U[0,1]" : 1.23888, ... }
    """
    replace = lambda key : re.sub(' *([LUT])\[ *([^ ]*) *, *([^ ]*) *\] *', 
              lambda match : match.group(1) + '[' + varnames[match.group(2)] + ',' + varnames[match.group(3)] + ']')
    return { replace(key) : value for key, value in parameter.items() }
           
  def _parseSamples(self,varnames,fname='samples.txt'):
    """
    Read the samples and return the names of parameters along with their values.
    Inputs:  varnames is a dictionary with keys being the variable names and values being an internal indexing
             fname    is the name of a file containing parameter data
    Output:  parameter is a key-value table from parameter names (using variable indexing) to numeric values, e.g.
                e.g. { "L[0,1]" : 2.34848, "U[0,1]" : 1.23888, ... }
    """
    with open(fname) as parameter_file:    
      named_parameter = json.load(parameter_file)
    return self._parseParameter(varnames,named_parameter)

  def _makeHillStrs(self,U,L,T,n,J):
    """
    Create the Hill function expressions
       neghill = "(U-L)*(T**n)/(X[J]**n+T**n) + L"
       poshill = "(U-L)*(X[J]**n)/(X[J]**n+T**n) + L"
       with the appropriate values for U, L, T, n, and J substituted
    """
    scalar = "("+U+"-"+L+")"
    Xn = "X["+J+"]**"+n
    Tn = T+"**"+n
    denom = "("+Xn+"+"+Tn+")"
    neghill=scalar+"*"+Tn+"/"+denom+" + "+ L
    poshill=scalar+"*"+Xn+"/"+denom+" + "+ L
    return neghill,poshill

  def _makeHillEqns(self,eqnstr,parameter,n):
    """
    Construct a lambda expression evaluation the right hand side of the Hill Model ODE
    Inputs:  eqnstr          -- a list of p-n format specification of the network node inputs
             parameternames  -- a list of parameter names, e.g. ["L[0,1]","U[0,1]",...]
             samples         -- a list of parameter values corresponding to the parameter names in 1-1 fashion
             n               -- Hill function exponent
    Output:  eqns            -- A list of lambda functions representing the right-hand-side of an ODE
                                The length of the list is the dimension of the ODE (which is also the length of eqnstr).
                                The ODE which is represented is d/dt x[i] = eqns[i]
    Implementation:
      This task is accomplished by reading the network specification file and replacing each variable
      in the input formulas with a suitable Hill function. The parsing is facilitated by having "eqnstr"
      in the form outputted by _parseEqns, e.g. 
         eqnstr == ["((0n)+(1p)*((2p))", ... ]
      and the algebraic syntax already present in the input formula already being suitable.
      We also add a decay term for each input formula.
         - X[0] + algebraic-combination of Hill functions
    """
    expression = "[";
    for k,e in enumerate(eqnstr):
      K = str(k)
      def replaceWithHillFunction(match):
        J = match.group(1)  # integer which indexes input variable
        regulation = match.group(2) # either "n" or "p"
        pair = "["+J+","+K+"]"
        U = parameter["U" + pair]
        L = parameter["L" + pair]
        T = parameter["T" + pair]
        neghill, poshill = self._makeHillStrs(U,L,T,n,J)
        return (poshill if match.group(2) == 'p' else neghill)
      # Include the formula into the expression
      expression += (',' if len(expression) > 1 else '') + "-X["+K+"]+" + re.sub('([0-9]*)([np])', replaceWithHillFunction, e)
    expression += ']'
    return eval('lambda X :' + expression)
