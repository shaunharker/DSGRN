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

import sys
import hillmodel as hm
reload (hm)
import pdb
import os

#python hillmodelscripts3.py '923669' '/home/mike/DSGRN-master/networks/5D_2016_01_28.txt' '/home/mike/timeseries_outputs/5D_2016_01_28/' '/home/mike/DSGRN_outputs/new/5D_2016_01_28/923669.txt'

def main(parameterstring,networkfile,savein_location,parameters_file):
    # choose a Hill exponent
    hillexp = 10

    #specifies the folder you'll save the graphs in, along with their file names
    
    #path of where output is saved
    savein = savein_location+'TimeSeries.txt'

    #the following creates the sample file of parameters (samplefile2) using DSGRN outputs (parameters_file)
    samples = os.getcwd() + '/samples.txt'
    f=open(parameters_file,'r')
    for line in f:
        if '->' in line:
            line = line.replace("{{","")
            line = line.replace("\n","")
            line = line.replace("}}","")
            file_ = open(samples, 'w')
            file_.write(line)
            file_.close()

    # make an instance of class hillmodel. Its attributes include the ODEs obtained from the network spec file and parameter values
    #obtained from the DSGRN outputs 
    HM_Instance = hm.hillmodel(networkfile,samples,hillexp)

    dim = HM_Instance.dim()

    # choose initial conditions and time period
    y0 = [1.0 for x in range(dim)] # range(W) for W variables in this network
    t0 = 0
    t1 = 30 # start at 0 and run for 10 time units
    dt = 0.01 # give me a new data point every hundredth of a time unit

    # run the Hill model- solved the ODEs and check for oscillations, and if all genes are oscillating, output the parameter node's
    #extrema ordering to a file
    times, timeseries = HM_Instance.simulateHillModel(savein,parameterstring,y0,t0,t1,dt) 

    # choose plotting options
    plotoptions={'linewidth':2}
    legendoptions={'fontsize':17,'loc':'upper left', 'bbox_to_anchor':(1, 1)} 
    # note: if you plt.savefig instead of plt.show, these options ensure that the 
    # legend won't be cut off, even if the legend labels are long
    figuresize = (15,10)

    # plot the results 
    #Example.plotResults(savein,savein_2,savein_3,times,timeseries,plotoptions,legendoptions,figuresize)

if __name__ == '__main__':
    main(sys.argv[1],sys.argv[2],sys.argv[3],sys.argv[4])