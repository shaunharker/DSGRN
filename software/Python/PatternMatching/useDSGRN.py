import itertools
import subprocess
import patternmatch
import pp,sys,time,os,glob,traceback
import fileparsers
from math import ceil

# Call this module as python useDSGRN.py NUMCPUS

# Given a Morse graph number MGN for a database DATABASEFILE (.db), do the following database searches:
# 
# The following gives the list of parameters in the file PARAMFILE (.txt)
# sqlite3 /share/data/CHomP/Projects/DSGRN/DB/data/DATABASEFILE.db 'select ParameterIndex from Signatures where MorseGraphIndex=MGN' > ./PARAMFILE
#
# The following finds the Morse set number:
# sqlite3 /share/data/CHomP/Projects/DSGRN/DB/data/DATABASEFILE.db 'select * from MorseGraphAnnotations where MorseGraphIndex=MGN'
#
# The following is a search for stable FCs:
# sqlite3 /share/data/CHomP/Projects/DSGRN/DB/data/DATABASEFILE.db 'select MorseGraphIndex,Vertex from MorseGraphAnnotations where Label="FC" except select MorseGraphIndex,Source from MorseGraphEdges'
#

def patternSearch(paramlist,patternfile='patterns.txt',networkfile="networks/5D_Model_B.txt",resultsfile='results_5D_B.txt',jsonbasedir='/share/data/bcummins/JSONfiles/',printtoscreen=0,printparam=0,findallmatches=1,unique_identifier='0'):
    # get domain cells for the network via shell call to dsgrn
    fname_domcells="{}dsgrn_domaincells_{}.json".format(jsonbasedir,unique_identifier)
    with open(fname_domcells, 'w',0) as f:
        subprocess.call(["dsgrn network {} domaingraph".format(networkfile)],shell=True,stdout=f)
    # call the pattern matcher for each parameter and record the results
    with open(resultsfile,'w',0) as R:
        paramcount=1
        for info in paramlist:
            morsegraph,morseset,param = info.split('|')
            if printparam and paramcount%1000==0:
                print str(paramcount)+' parameters checked'
                sys.stdout.flush()
            paramcount+=1
            # shell call to dsgrn to produce json files as input for the pattern matcher
            fname_domgraph='{}dsgrn_domaingraph_{}.json'.format(jsonbasedir,unique_identifier)
            with open(fname_domgraph, 'w',0) as f:
                subprocess.call(["dsgrn network {} domaingraph json {}".format(networkfile,param)],shell=True,stdout=f)
            fname_morseset='{}dsgrn_output_{}.json'.format(jsonbasedir,unique_identifier)
            with open(fname_morseset, 'w',0) as f:
                subprocess.call(["dsgrn network {} analyze morseset {} {}".format(networkfile,morseset,param)],shell=True,stdout=f)
            try:
                patterns,matches=patternmatch.callPatternMatch(fname_morseset=fname_morseset,fname_domgraph=fname_domgraph,fname_domcells=fname_domcells,fname_patterns=patternfile,fname_results=resultsfile,writetofile=0,returnmatches=1,printtoscreen=printtoscreen,findallmatches=findallmatches)
                for pat,match in zip(patterns,matches):
                    if findallmatches:
                        R.write("Parameter: {}, Morse Graph: {}, Morse Set: {}, Pattern: {}, Results: {}".format(param,morsegraph,morseset,pat,match)+'\n')
                    else:
                        R.write("Parameter: {}, Morse Graph: {}, Morseset: {}, Pattern: {}".format(param,morsegraph,morseset,pat)+'\n')
            except Exception as e:
                print 'Problem parameter is {}'.format(param)
                print traceback.format_exception_only(type(e), e) 
                sys.stdout.flush()
        removeFiles([fname_domgraph,fname_morseset])
    removeFiles([fname_domcells])

def removeFiles(listoffiles):
    for f in listoffiles:
        subprocess.call(['rm '+f],shell=True)           

def mergeFiles(mergefile,splitfiles):
    with open(mergefile,'a') as m:
        for sfile in splitfiles:
            with open(sfile,'r') as f:
                for l in f.readlines():
                    m.write(l)
    removeFiles(splitfiles)

def calculateNumberofJobs(numparams,ncpus,maxslice=500):
    paramspercpu=int(ceil(float(numparams)/ncpus))
    paramsperslice=min([paramspercpu,maxslice])
    numjobs=int(ceil(float(numparams)/paramsperslice))
    return numjobs, paramsperslice

def parallelrun(job_server,numparams,allparamsfile,resultsfile,allresultsfile,ncpus,patternfile,networkfile,jsonbasedir,printtoscreen=0,printparam=0,findallmatches=0):
    numjobs,paramsperslice=calculateNumberofJobs(numparams,ncpus)
    jobs=[]
    allsubresultsfiles=[]
    with open(allparamsfile,'r') as apf:
        for n in range(numjobs):
            head = [line for line in [apf.readline() for _ in range(paramsperslice)] if len(line) ]
            unique_identifier='{:04d}'.format(n)
            subresultsfile=resultsfile+unique_identifier+'.txt'
            allsubresultsfiles.append(subresultsfile)
            jobs.append(job_server.submit(patternSearch,(head,patternfile,networkfile,subresultsfile,jsonbasedir,printtoscreen,printparam,findallmatches,unique_identifier), depfuncs=(),modules = ("subprocess","patternmatch", "preprocess","fileparsers","walllabels","itertools","numpy","json","traceback","sys"),globals=globals()))
    print "All jobs starting."
    sys.stdout.flush()
    for job in jobs:
        job()
    job_server.destroy()
    print "All jobs ended."
    sys.stdout.flush()
    # mergeFiles(allresultsfile,allsubresultsfiles)
    # print "Results files merged."
    # sys.stdout.flush()

def concatenateParams(allparamsfile,morse_graphs_and_sets):
    numparams=0
    with open(allparamsfile,'w') as apf:
        for (mgraph,mset) in morse_graphs_and_sets:
            with open('param_temp.txt','w',0) as p:
                subprocess.call(["sqlite3 /share/data/CHomP/Projects/DSGRN/DB/data/{}.db 'select ParameterIndex from Signatures where MorseGraphIndex={}'".format(networkfilename,mgraph)],shell=True,stdout=p)
            with open('param_temp.txt','r') as p:
                for ms in mset:
                    for param in p.readlines():
                        numparams+=1
                        apf.write('{}|{}|{}'.format(mgraph,ms,param))
            removeFiles(['param_temp.txt'])
    return numparams

def main_conley3_filesystem_allparameters(patternsetter,getMorseGraphs,networkfilename="5D_2015_09_11",morsegraphselection="stableFCs",ncpus=1,printtoscreen=0,printparam=0,findallmatches=0):
    # find all morse graphs with desired characteristic
    networkfilebasedir="/share/data/bcummins/DSGRN/networks/"
    morsegraphfile="/share/data/bcummins/morsegraphs/"+networkfilename+'_'+morsegraphselection+'_listofmorsegraphs.txt'
    getMorseGraphs(networkfilename,morsegraphfile)
    # set patterns
    patternfile='/share/data/bcummins/patterns/patterns_'+networkfilename+'.txt'
    patternsetter(patternfile)
    # set other file names and paths
    allparamsfile="/share/data/bcummins/parameterfiles/"+networkfilename+'_'+morsegraphselection+'_concatenatedparams.txt'
    resultsbasedir='/share/data/bcummins/parameterresults/'
    resultsfile=resultsbasedir+networkfilename
    allresultsfile=resultsfile+'_'+morsegraphselection+'_allresults.txt'
    jsonbasedir='/share/data/bcummins/JSONfiles/'
    # parse morse graphs
    morse_graphs_and_sets=fileparsers.parseMorseGraphs(morsegraphfile)
    # get all parameters from all morse graphs
    numparams=concatenateParams(allparamsfile,morse_graphs_and_sets)
    # split parameter file into one for each cpu
    # ncpus=splitParams(paramfile+'_params_',numparams,ncpus,allparamsfile)
    return [numparams,allparamsfile,resultsfile+'_results_',allresultsfile,ncpus,patternfile,networkfilebasedir+networkfilename+".txt",jsonbasedir,printtoscreen,printparam,findallmatches]

def setPattern_Malaria_20hr_2015_09_11(patternfile):
    with open(patternfile,'w') as f:
        for s1 in itertools.permutations(['x3 max','x5 max','x1 max', 'x4 max']):
            patternstr1=', '.join(s1) + ', x2 max, '
            for s2 in itertools.permutations(['x1 min', 'x3 min','x5  min', 'x2 min','x4 min']):
                patternstr2=patternstr1 + ', '.join(s2) + ', ' + s1[0] + '\n'
                f.write(patternstr2)

def setPattern_5D_Cycle(patternfile):
    with open(patternfile,'w',0) as f:
        f.write('X3 max, X4 max, X3 min, X4 min\n X3 max, X4 min, X3 min, X4 max')

def setPattern_3D_Example(patternfile):
    with open(patternfile,'w',0) as f:
        f.write('Z min, X min, Y min, Z max, X max, Y max\n X max, Y max, Z max, X min, Y min, Z min\n X min, Y max, Z min, X max, Y min, Z max\n X max, Y min, Z max, X min, Y max, Z min')

def setPattern_3D_Cycle(patternfile):
    with open(patternfile,'w',0) as f:
        f.write('X3 min, X1 min, X2 min, X3 max, X1 max, X2 max\n X1 max, X2 max, X3 max, X1 min, X2 min, X3 min\n X1 min, X2 max, X3 min, X1 max, X2 min, X3 max\n X1 max, X2 min, X3 max, X1 min, X2 max, X3 min \n X1 max, X3 max, X1 min, X3 min\n X1 max, X3 max, X2 max, X1 min, X3 min, X2 min')

def selectStableFC(networkfile,morsegraphfile):
    subprocess.call(['''sqlite3 /share/data/CHomP/Projects/DSGRN/DB/data/{}.db 'select MorseGraphIndex,Vertex from MorseGraphAnnotations where Label="FC" except select MorseGraphIndex,Source from MorseGraphEdges' > {}'''.format(networkfile,morsegraphfile)],shell=True)

if __name__=='__main__':
    morsegraphselection="stableFCs"
    getMorseGraphs=selectStableFC
    # networkfilename="5D_Cycle"
    # patternsetter=setPattern_5D_Cycle
    # networkfilename="5D_2015_09_11"
    # patternsetter=setPattern_Malaria_20hr_2015_09_11
    networkfilename="3D_Cycle"
    patternsetter=setPattern_3D_Cycle

    listofargs=main_conley3_filesystem_allparameters(patternsetter,getMorseGraphs,networkfilename,morsegraphselection,int(sys.argv[1]),printtoscreen=0,printparam=0,findallmatches=0)
    job_server = pp.Server(ncpus=0,ppservers=("*",))
    time.sleep(30)
    parallelrun(job_server,*listofargs)
