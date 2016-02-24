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
import pdb
from decimal import Decimal

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
        parameternames,samples = self._parseSamples2(self.varnames,samplefile)
        self.eqns=self._makeHillEqns(eqnstr,parameternames,samples,hillexp)

    def simulateHillModel(self,savein_ts,savein_ts_2,savein_eqns,parameterstring,initialconditions,initialtime,finaltime,timestep):
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
  
        global saveFC
        saveFC = 0
        global saveXC
        saveXC = 0
        output=[]
        max_dict={} #dict of dicts
        min_dict={}
        for k in range(len(varnames2)):
          """
          signal = 0    #starts by assuming not FC 
          for increment in range(10):
            if abs(timeseries[2500][k]-timeseries[2500+(increment*10)][k]) > 10**-2: # if there's at least one bump in the section
              signal = 1
              break    
          if signal == 1:
          """
          #when at FP, may have value 0.33334 and the next value be 0.33335, so compare differences instead of truncate and rounding
          if abs(timeseries[2500][k]-timeseries[2600][k]) > 10**-2 or abs(timeseries[2500][k]-timeseries[2650][k]) > 10**-2:
            #plot's end behavior is FC
            gene_max=[0,0] #time,value
            gene_min=[0,10**15]
            gene_values={}
            for time in range(len(timeseries[2000:])): #relative time, not absolute time
              if timeseries[2000:][time][k] > gene_max[1]:
                gene_max[0] = time
                gene_max[1] = timeseries[2000:][time][k]
              if timeseries[2000:][time][k] < gene_min[1]:
                gene_min[0] = time
                gene_min[1] = timeseries[2000:][time][k]
            max_dict[k] = gene_max
            min_dict[k] = gene_min
        #if (not max_dict) == False and (not min_dict) == False: #must have both max and min else it's not an oscillation
        if max_dict != {} and min_dict != {}:
          output.append(parameterstring)
          saveXC=1
          for time in range(len(timeseries[2000:])): #relative time, not absolute time
            for k in range(len(varnames2)): #time is the outer loop so we can have 1max,2max,1min (etc)
              if k in max_dict:
                if timeseries[2000:][time][k] == max_dict[k][1] and (str(k)+' max') not in output: 
                  output.append(str(k)+' max') #must have space or else replacing w/ varnames won't work
                if timeseries[2000:][time][k] == min_dict[k][1] and (str(k)+' min') not in output:
                  output.append(str(k)+' min')
          for j in range(len(output)):
            extrema = output[j]
            extrema_list = extrema.split()
            for i in range(len(extrema_list)):
              for k,v in enumerate(varnames2):
                if extrema_list[i] == str(k):       
                  extrema_list[i] = v
            output[j] = (' '.join(extrema_list))
          file1 = open(savein_ts, 'a')
          file1.write(str(output).replace('[','').replace(']','').replace("'","")+"\n")
          file1.close()
          if len(output) == 11: #1 + 2*D
            saveFC = 1
            file2 = open(savein_ts_2, 'a')
            file2.write(str(output).replace('[','').replace(']','').replace("'","")+"\n")
            file2.close()
            """
            file3 = open(savein_eqns, 'a')
            file3.write("\n")
            file3.write(parameterstring+': '+str(debug_only)+"\n")      
            file3.close()
            """
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
      f=open(fname,'r')
      global varnames2 #so it can be use in simulateHillModel
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
      varnames2 = varnames
      return eqnstr,varnames
        
    def _parseSamples2(self,varnames,fname='samples.txt'):
        # Private parser.
        f=open(fname,'r')
        pnames=[]
        samples_frac=[]
        lines = f.readlines()
        K=lines[0].split()
        #K=lines[1].split()
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
        eqns=[]
  		global debug_only
  		debug_only = []
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
      		debug_only.append(e)
            eqns.append(eval(e))
  			#pdb.set_trace()
        return eqns
