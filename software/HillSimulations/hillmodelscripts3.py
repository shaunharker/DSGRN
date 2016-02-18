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

import sys
import hillmodel as hm
reload (hm)
import pdb
import os

#BEFORE RUNNING: CHANGE PATH NAMES OR DELETE PREV VERSIONS OF EXTREMA.TXT, FC_EXTREMA.TXT AND FC_EQNS.TXT BECAUSE THESE USE FILE(PATH,'a')

def main(networkname,pnMG):
    pnMG_dict = {}
    parameterlist =[]
    f=open(pnMG,'r')
    for line in f:
        splitted = line.split() 
        pnMG_dict[splitted[1]] = splitted[0]
        parameterlist.append(splitted[1])
    counter = 0
    for p in parameterlist:
        counter += 1
        HMS(p,networkname,pnMG_dict[p])
        if counter % 500 == 0:
                print(counter)

def HMS(parameterstring,networkname,morsegraph):
    # specify the full path to your file, unless it is in the same directory
    networkfile = '/home/mike/DSGRN-master/networks/'+networkname+'.txt'

    # choose a Hill exponent
    hillexp = 10

    #specifies the folder you'll save the graphs in, along with their file names
    
    #path of where output is saved
    savein_ts0 = '/home/mike/timeseries_outputs/'+networkname
    savein_ts = '/home/mike/timeseries_outputs/'+networkname+'/'+networkname+'_extrema_n10_3.txt'
    savein_ts_2 = '/home/mike/timeseries_outputs/'+networkname+'/'+networkname+'_FC_extrema_n10_3.txt'
    
    savein_eqns0 = '/home/mike/eqn_FC_outputs/'+networkname
    savein_eqns = '/home/mike/eqn_FC_outputs/'+networkname+'/'+networkname+'_FC_eqns_n10_2.txt'

    #alt = 'Seagate Expansion Drive/z ubuntu'
    alt = 'Cruzer'
    savein0 = '/media/mike/'+alt+'/HillModel_outputs_n10/'+networkname+'/MG'+morsegraph
    savein =  '/media/mike/'+alt+'/HillModel_outputs_n10/'+networkname+'/MG'+morsegraph+'/'+networkname+'_pn'+parameterstring+'.png'
    savein0_2 = '/media/mike/'+alt+'/HillModel_FC_outputs_n10/'+networkname+'/MG'+morsegraph
    savein_2 = '/media/mike/'+alt+'/HillModel_FC_outputs_n10/'+networkname+'/MG'+morsegraph+'/'+networkname+'_pn'+parameterstring+'.png'
    savein0_3 = '/media/mike/'+alt+'/HillModel_XC_outputs_n10/'+networkname+'/MG'+morsegraph
    savein_3 = '/media/mike/'+alt+'/HillModel_XC_outputs_n10/'+networkname+'/MG'+morsegraph+'/'+networkname+'_pn'+parameterstring+'.png'


    if not os.path.exists(savein0):
            os.makedirs(savein0) #if a folder AND its child folder don't exist, this creates both of them
    if not os.path.exists(savein0_2):
            os.makedirs(savein0_2)
    if not os.path.exists(savein0_3):
            os.makedirs(savein0_3)
    if not os.path.exists(savein_ts0):
            os.makedirs(savein_ts0)
    if not os.path.exists(savein_eqns0):
            os.makedirs(savein_eqns0)

    #the following creates the sample file of parameters (samplefile2) using DSGRN outputs (raw_samples)
    raw_samples = '/home/mike/DSGRN_outputs/new/'+networkname+'/'+parameterstring+'.txt'
    
    samples = '/home/mike/DSGRN_outputs/samples.txt'
    f=open(raw_samples,'r')
    for line in f:
        if '->' in line:
            line = line.replace("{{","")
            line = line.replace("\n","")
            line = line.replace("}}","")
            file_ = open(samples, 'w')
            file_.write(line)
            file_.close()
    

    samplefile2 = '/home/mike/DSGRN_outputs/samples.txt'

    # make an instance of class hillmodel
    Example = hm.hillmodel(networkfile,samplefile2,hillexp)

    dim = ''
    for i in networkname:
        if i == 'D':
            break
        else:
            dim += i
    dim=int(dim)

    # choose initial conditions and time period
    y0 = [1.0 for x in range(dim)] # range(W) for W variables in this network
    t0 = 0
    t1 = 30 # start at 0 and run for 10 time units
    dt = 0.01 # give me a new data point every hundredth of a time unit

    # run the Hill model
    times, timeseries = Example.simulateHillModel(savein_ts,savein_ts_2,savein_eqns,parameterstring,y0,t0,t1,dt) 

    # choose plotting options
    plotoptions={'linewidth':2}
    legendoptions={'fontsize':17,'loc':'upper left', 'bbox_to_anchor':(1, 1)} 
    # note: if you plt.savefig instead of plt.show, these options ensure that the 
    # legend won't be cut off, even if the legend labels are long
    figuresize = (15,10)

    # plot the results 
    Example.plotResults(savein,savein_2,savein_3,times,timeseries,plotoptions,legendoptions,figuresize)

if __name__ == '__main__':
    main(sys.argv[1],sys.argv[2])
