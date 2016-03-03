# The MIT License (MIT)

# Copyright (c) 2016 Breschine Cummins
# Modified by Michael Lan

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
import pdb
from decimal import Decimal
import collections

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
        self.d=len(eqnstr)

    def dim(self):
        #Dimension, or total # of genes in network
        return self.d

    def checkForOscillations(self,savein,parameterstring,timeseries):
        """
        This method relies on several assumptions, namely: We only desire oscillations with consistent amplitudes and periods. In other 
        words, all the max in the end behavior of the function are equal, and same with the min.

        For each gene's function, take its end slice, assuming that at this end slice the function would have already settled into the 
        desired behavior. Going from left to right in the end slice, just record each max/min based on whether the area around that point 
        changes its direction of increase or decrease (the basic definition of an extrema point; checking increasing or decreasing is done 
        just by seeing if adjacent points are bigger/smaller). 

        After this is done, it checks if the global max and min are within a degree of error to one another. If they are, that function 
        has probably gone to a fixed point, so conclude the function is not oscillating. Since we assume that we only want one unique 
        max and min in this end slice, if a min does not equal the global min within a degree of error, the behavior of that function
        is not considered a 'desired oscillation'; same with comparing each max with the global max. Later, another add-on to the method
        may be: If periods (distance b/w max/min) do not match, that function may also be ruled out as a 'desired oscillation'.

        So far this new method has been tested on the parameter node '923669' and found all its genes are oscillating. The extrema
        ordering that this method outputs also matches one of the experimental extrema ordering patterns
        """
        output=[]
        output.append(parameterstring)
        time_of_extrema={} #contains the extrema of every single gene, stored in the format time:k max or min. Repeated extrema are included.
        numOfGenes=0 #records how many genes are oscillating
        for k in range(len(self.varnames)):
          #the first check to make sure function didn't go to fixed point. If fails, avoid performing lengthy loop on end slice of gene's function  
          #uses 'or' because the function's period may be at 10 timesteps; if so, it won't be at 15 timesteps. Prevents eval oscillations as fixed points
          if abs(timeseries[-1000:][time][k] - timeseries[-990:][time][k]) > 10**-3 or abs(timeseries[-1000:][time][k] - timeseries[-985:][time][k]) > 10**-3: 
              extrema_order_k={} #records all extrema for gene k in time:value format
              list_of_max=[] #to be used to find # of outliers
              list_of_min=[]
              slope = ''
              for i in range(999):
                if slope == 'decreasing' and (timeseries[-1000+i][k] - timeseries[-1000+i+1][k])<0:
                  extrema_order_k[-1000+i] = timeseries[-1000+i][k]
                  list_of_min.append(timeseries[-1000+i][k])
                  slope = 'increasing'
                elif slope == 'increasing' and (timeseries[-1000+i][k] - timeseries[-1000+i+1][k])>0:
                  extrema_order_k[-1000+i] = timeseries[-1000+i][k]
                  list_of_max.append(timeseries[-1000+i][k])
                  slope = 'decreasing'
                elif (timeseries[-1000+i][k] - timeseries[-1000+i+1][k])>0 and slope == '':
                  slope = 'decreasing'
                elif (timeseries[-1000+i][k] - timeseries[-1000+i+1][k])<0 and slope == '':
                  slope = 'increasing'
              
              #loop through and compare each max/min to the max/min. If too many outlier extrema, reject the oscillation as 'undesired'
              #Question: what if 3 outliers are adjacent to each other in list_of_max? Then the ones on this interval's boundaries are not outliers,
              #so at least 2 of the 3 outliers will be detected. Since if there are >2 outliers oscillation is rejected, this seems okay.
              num_of_outliers_max = 0
              for index in range(len(list_of_max)-1):
                if abs(list_of_max[index] - list_of_max[index+1]) > 10**-3: #if True, one of the two is outlier; doesn't matter which b/c just want count # of outliers
                  num_of_outliers_max += 1
              num_of_outliers_min = 0
              for index in range(len(list_of_min)-1):
                if abs(list_of_min[index] - list_of_min[index+1]) > 10**-3: #if True, one of the two is outlier; doesn't matter which b/c just want count # of outliers
                  num_of_outliers_min += 1
              if num_of_outliers_max < 2 and num_of_outliers_min <2:
                values=extrema_order_k.values()
                global_max = max(values)
                global_min = min(values)
                if abs(global_max - global_min) > 10**-3: #second check: if True, function even more likely didn't go to a fixed point
                  numOfGenes+=1
                  for time in extrema_order_k:
                    if abs(extrema_order_k[time]-global_max) < 10**-3:
                      time_of_extrema[time] = str(k)+' max' #if extrema close to global max, add it to outputs
                    if abs(extrema_order_k[time]-global_min) < 10**-3:
                      time_of_extrema[time] = str(k)+' min'
        if numOfGenes == self.dim():      #Checks if all genes oscillated
          #rearrange all extrema of every gene according to where they are in time
          ordered_extrema = collections.OrderedDict(sorted(time_of_extrema.items())) 
          for time in ordered_extrema:
            if ordered_extrema[time] not in output: #checks for the cyclic pattern. Repeated extrema not included.
              output.append(ordered_extrema[time])
          for j in range(len(output)): #replace index number, used in timeseries, with parameter's name
            extrema = output[j]
            extrema_list = extrema.split()
            for i in range(len(extrema_list)):
              for k,v in enumerate(self.varnames):
                if extrema_list[i] == str(k):       
                  extrema_list[i] = v
            output[j] = (' '.join(extrema_list))
          file = open(savein, 'a')
          file.write(str(output).replace('[','').replace(']','').replace("'","")+"\n")
          file.close()
          
    def old_checkForOscillations(self,savein,parameterstring,timeseries):
        """
        The procedure below will be fixed later once a better one is found:
        Checks to see if ODE solutions do not go to fixed points. First, it checks to see if there are any bumps at the end 
        of the solution (at high time values). If there are, then for each gene, it finds their extrema. Next, it goes through the 
        list 'timeseries' and sees which extrema come first as time goes from 0 to infinity. It records the order the extrema appear
        at and puts the ordering in a list 'output'. If all genes were oscillating, this parameter node's extrema ordering is written 
        to a file, located in path 'savein'

        Structure of list 'timeseries':
        timeseries[time][value of each gene at time]

        e.g. timeseries[5] gives [0.5,0.4,0.3], which is a list of the values of each gene at time=5. Each gene is indexed by an 
        integer. For instance, p53 is indexed as '1', so timeseries[5][1] would correspond to p53's value
        """
        output=[]
        max_dict={}
        min_dict={}
        for k in range(len(self.varnames)):
          signal = 0    #starts by assuming no bumps detected
          for increment in range(100): #n of range(n) must be sufficiently large to detect bumps
            #when at FP, may have value 0.33334 and the next value be 0.33335, so compare differences instead of truncate and rounding
            if abs(timeseries[-1000][k]-timeseries[-1000+(increment*10)][k]) > 10**-2: # if there's at least one bump in the section
              signal = 1
              break    
          if signal == 1:
            #Gene_max was a list that recorded both time and value b/c that was for debugging purposes; after debugging I forgot to change it
            gene_max=0
            gene_min=10**15
            gene_values={}
            for time in range(len(timeseries[-1000:])): #relative time, not absolute time
              if timeseries[-1000:][time][k] > gene_max:
                gene_max = timeseries[-1000:][time][k]
              if timeseries[-1000:][time][k] < gene_min:
                gene_min = timeseries[-1000:][time][k]
            max_dict[k] = gene_max
            min_dict[k] = gene_min
        if max_dict != {} and min_dict != {}:
          output.append(parameterstring)
          for time in range(len(timeseries[-1000:])): #relative time, not absolute time
            for k in range(len(self.varnames)): #time is the outer loop so we can have 1max,2max,1min (etc)
              if k in max_dict:
                if abs(timeseries[-1000:][time][k] - max_dict[k]) < 10**-5 and (str(k)+' max') not in output: 
                  output.append(str(k)+' max') #must have space or else replacing w/ varnames won't work
                if abs(timeseries[-1000:][time][k] == min_dict[k]) < 10**-5 and (str(k)+' min') not in output:
                  output.append(str(k)+' min')
          for j in range(len(output)):
            extrema = output[j]
            extrema_list = extrema.split()
            for i in range(len(extrema_list)):
              for k,v in enumerate(self.varnames):
                if extrema_list[i] == str(k):       
                  extrema_list[i] = v
            output[j] = (' '.join(extrema_list))
          if len(output) == (1+2*self.dim()): #Checks if all genes oscillated, and thus had their extrema recorded in output
            file = open(savein, 'a')
            file.write(str(output).replace('[','').replace(']','').replace("'","")+"\n")
            file.close()

    def simulateHillModel(self,savein,parameterstring,initialconditions,initialtime,finaltime,timestep):
        '''
        Simulate the constructed Hill model for a given set of initial conditions 
        and time period. The given time step only specifies which output timeseries
        is returned. The time step for the backwards difference ODE solver is 
        determined by the algorithm.
        '''
        def RHS(t,x,eqns):
            xdot = [e(x) for e in eqns]
            return np.array(xdot)
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
  
        self.checkForOscillations(savein,parameterstring,timeseries)

        return times,timeseries

    def plotResults(self,savein,savein_2,savein_3,times,timeseries,plotoptions={},legendoptions={},figuresize=()):
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
        fig = plt.figure(figsize=figuresize)
      timeseries=np.array(timeseries)
      for k in range(timeseries.shape[1]):
        plt.plot(times,timeseries[:,k],label=self.varnames[k],**plotoptions)
      plt.legend(**legendoptions)
      plt.axis('tight')
      #plt.savefig(savein)
      if saveFC == 1:
        plt.savefig(savein_2)
      if saveXC==1 and saveFC == 0:
        plt.savefig(savein_3)
      #plt.show()
      plt.close(fig)

    def _parseEqns(self,fname='equations.txt'):
      # Private parser
      #'if line [i]' lines are for inconsistent formats in equations.txt. Easier way is to modify equations.txt to consistent format
      #IE) Change 'x:x+y" into 'x : x + y' before running. Change ') (' into ')(', etc.
      """
      Takes the network file (in network spec format) and returns eqnstr, varnames. Eqnstr is a list of ODEs, one for each gene, in the
      p-n format (explained below). Varnames maps each gene name to an index number, which makes it easier to match each gene's ODE
      with its set of parameters by using enum functions (as seen in makeHillEqns)

      For example, if a gene W has the equation (~X + Y)(Z), and varnames =[X,Y,Z], then its ODE in eqnstr would be (0n + 1p)(1p). 
      X, which has index 0 as it's in the 0th index in varnames, represses W due to the symbol ~, and thus is assigned '0n'. 
      Y activates W and thus is assigned '1p' in p-n format.

      For 'e in eqns', e.split() was used because without .split(), using replace made the program encounter errors in cases such as
      matching '10' with the appropriate gene name, as '10' contains '0' so instead of matching '10' to the gene name at index 10,
      it would match it with the gene name at index 0, etc. Each time I encountered an error I used .split() on the string and parsed
      it as a list instead. This was done in parseSamples and makeHillEqns too.

      The network spec files had some formatting inconsistencies; e.g. a gene X might have X:Z+Y , while all other genes would be in
      the format Y : ~W. The parser addresses these inconsistencies whenever they were encountered; however, they do not address all
      potential inconsistencies.

      """
      f=open(fname,'r')
      varnames=[]
      eqns=[]
      for line in f:
        for i in range(len(line)):
          if line[i] == ':':
            if line[i-1] != ' ':
              line = line.replace(":", " :")
            if line[i+1] != ' ':  #must split into 2 for cases like 'Y2: Y1' as in 4D_Example. Use if, not elif
              line = line.replace(":", ": ")
        L=line.split(' : ')
        if len(L) > 1: #for cases when L = ['\n'] as seen for case 5D_2015_10_23.txt
          varnames.append(L[0]) 
          equation = L[1]             
          #the following line (only 1 line) is for ubuntu only b/c appends X2\n; in windows, X2\n auto interpreted as X2     
          equation = equation.replace('\n','')
          equation = equation.replace(' ('," * (").replace(')(',") * (") #IE) 3D_Cycle's 224.txt has (X1)(~X2) as an eqn
          eqns.append(equation)
      f.close()
      eqnstr=[]
      for e in eqns:
        for i in range(len(e)):
          if e[i] == '+':
            if e[i-1] != ' ':
              e = e.replace("+", " +")
            if e[i+1] != ' ':
              e = e.replace("+", "+ ")
          elif e[i] == '*':
            if e[i-1] != ' ':
              e = e.replace("*", " *")
            if e[i+1] != ' ':
              e = e.replace("*", "* ")
        e_list = e.split()  #list used instead of string for cases like network 5D_2016_01_16_C whose genes are numbers (k is # too)
        e2 = e.replace(')',"").replace('(',"") #keep paren. in e, but remove from e2. e2_list only used for 'if v in...'
        e2_list = e2.split()  #e2 is used for cases w/ gene eqs in the format (X+Y)(~Z)
        for k,v in enumerate(varnames):
          if v in e2_list or '~'+v in e2_list: #don't split and rejoin if no need to      
            for i in range(len(e2_list)):
              if e2_list[i] == v: #check for in e2_list, but replace in e_list. Use .replace to keep parenthesis
                e_list[i] = e_list[i].replace(v,str(k)+' p') 
              elif e2_list[i] == '~'+v: #safe to use .replace b/c only changes current position in list, not entire list/string
                e_list[i] = e_list[i].replace('~'+v,str(k)+' n')
              if e_list[i] == '+' or e_list[i] == '*':
                e_list[i] = ' '+e_list[i]+' '
            new_e = ''.join(e_list)
        eqnstr.append(new_e)
      return eqnstr,varnames
        
    def _parseSamples(self,varnames,fname='samples.txt'):
        """
        This takes the DSGRN output, the solutions to the CAD inequalities, and returns lists 'parameternames' and 'samples'. 
        
        'Parameternames' takes the each parameter, such as L[X,Y], and replaces each gene name by its index name, as described
        in 'varnames' in the function parseEqns's comments. So L[X,Y] will become L[0,1] and added to 'parameternames'

        'Samples' is the corresponding value for each parameter. The parameter and its value are mapped to each other through the
        index positions in both lists. For example, L[X,Y]=0.5, so parameternames[0]=L[X,Y] and samples[0]=0.5

        The DSGRN output has the values as fractions so they were converted to decimals
        """
        # Private parser.
        f=open(fname,'r')
        pnames=[]
        samples_frac=[]
        lines = f.readlines()
        K=lines[0].split()
        prefix = ''
        for word in K[:-1]:
            if '[' in word: #L[X,
                prefix = word
            elif ']' in word: #Y]
                pnames.append(prefix + ' ' + word)
            elif word[:-1].isdigit() == True or '/' in word and '[' not in word and ']' not in word:
              samples_frac.append(word[:-1])
        samples_frac.append(K[-1])
        parameternames=[]
        samples=[]
        for p in pnames: #for networks whose genes are numbers, list should also be used
          p_list = p.split()    
          prefix2 = ''
          suffix2 = ''
          for i in range(len(p_list)):
              for k,v in enumerate(varnames): #must do it twice b/c suffix may be replaced before prefix  
                if p_list[i].replace('L[',"").replace(',',"") == v:
                  prefix2 = 'L['+str(k)
                elif p_list[i].replace('U[',"").replace(',',"") == v:
                  prefix2 = 'U['+str(k)
                elif p_list[i].replace('THETA[',"").replace(',',"") == v:
                  prefix2 = 'THETA['+str(k)
                elif p_list[i].replace(']',"") == v:
                  suffix2 = ','+str(k)+']'
          parameternames.append(prefix2 + suffix2)
        for value in samples_frac:
            if '/' in value:
                numerator = ''
                denominator = ''
                denomtime = False
                for i in value: #convert fraction to decimal so simulateHillModel can work
                    if i.isdigit() == True and denomtime == False:
                        numerator = numerator + i
                    elif i == '/':
                        denomtime = True
                    elif i.isdigit() == True and denomtime == True:
                        denominator = denominator + i
                decimal = float(numerator)/float(denominator)
                samples.append(str(decimal))
            else:
                samples.append(value)
        return parameternames,samples

    def _makeHillStrs(self,U,L,T,n,J):
        # Private constructor.
        scalar = "("+U+"-"+L+")"
        Xn = "X["+J+"]**"+n
        Tn = T+"**"+n
        denom = "("+Xn+"+"+Tn+")"
        neghill=scalar+"*"+Tn+"/"+denom+" + "+ L
        poshill=scalar+"*"+Xn+"/"+denom+" + "+ L
        return neghill,poshill

    def _makeHillEqns(self,eqnstr,parameternames,samples,n):
        # Private constructor.
        # X is not yet defined; eval a lambda function
        """
        After obtaining the ODEs from the network spec format and the parameter's names and values from the DSGRN solution file, this
        function places the parameters in each ODE. Eqnstr has each parameter in p-n format, e.g. 0p+10n, so replace each index # with
        the appropriate hill functions and parameters (IE 0p would have the hill function corresponding to activation, and since 0
        maps with p53, then place parameters with the format L[X,p53] into the right places, since these parameters show how other 
        genes affect p53)

        This function first matches 0p with its parameters. Then it reads if it's p or n, and chooses the appropriate hill function
        using 'makeHillStrs'; however, before replacing p or n with the hill function, there is some other parsing to do. Due to
        errors encountered that were mentioned in parseEqns's comments, each string 'e' (an equation from eqnstr) was parsed as a list
        instead of as a string. 
        """
        eqns=[]
        for k,e in enumerate(eqnstr):
            e2 = e.replace(')',"").replace('(',"") #keep paren. in e, but remove from e2. e2 is used for if statements
            #e2 is used for cases w/ gene eqs in the format (X+Y)(~Z)
            e_orig = e2.split()  #n and p are replaced during J loop, so 'if J in e' cond must use original copy of e, not changed e
            K=str(k)
            e_list = e.split()
            for j in range(len(eqnstr)):
                J=str(j)
                if J in e_orig: #IE) when replacing 0p+10p, all '0 p' are replaced, so use list instead of str to replace
                    # if j affects k, find U,L,T in parameternames
                    U,L,T=None,None,None
                    for p,v in zip(parameternames,samples):
                        if J+','+K in p:
                            exec(p[0]+"= str(v)")
                        if filter(None,[U,L,T])==[U,L,T]: 
                            #quit when U,L,T assigned
                            break
                    # substitute the negative and positive hill string
                    neghill,poshill=self._makeHillStrs(U,L,T,str(n),J)
                 
                    for i in range(len(e_list) - 1): #-1 b/c [i+1]. This does replacement
                        if e_orig[i] == J and e_orig[i+1] == 'p':
                          e_list[i] = e_list[i].replace(J,poshill)
                          if ')' not in e_list[i+1]: #don't delete ')' from ')n' or ')p'
                            del e_list[i+1]
                            del e_orig[i+1] #make e_orig retain same index of #s and p/n's as e_list
                          else:
                            e_list[i+1] = e_list[i+1].replace('p','')
                        elif e_orig[i] == J and e_orig[i+1] == 'n':
                          e_list[i] = e_list[i].replace(J,neghill)
                          if ')' not in e_list[i+1]: #don't delete ')' from ')n' or ')p'
                            del e_list[i+1]
                            del e_orig[i+1] #make e_orig retain same index of #s and p/n's as e_list
                          else:
                            e_list[i+1] = e_list[i+1].replace('n','')
            e = ''.join(e_list)
            # make a lambda function for each equation
            e="lambda X: -X["+K+"] + " + e
            eqns.append(eval(e))
        return eqns
