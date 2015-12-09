import itertools
import subprocess
import patternmatch
import pp,sys,time,traceback
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

def patternSearch(networkfile,jsonbasedir,unique_identifier,paramfile,resultsfile,patternfile,printtoscreen,findallmatches):
    # had to completely rewrite this function from scratch (copying it), in order to get 
    # subprocess.call to redirect to a file. I wrote other short functions with the same
    # commands and didn't have this issue. Could there have been invisible characters?
    # At any rate, this is a potential source of failure in the future, since I have no
    # idea what happened.
    fname_domcells = "{}dsgrn_domaincells_{}.json".format(jsonbasedir,unique_identifier)
    fileWrite(networkfile,fname_domcells,'domaingraph')
    with open(resultsfile,'w',0) as R, open(paramfile,'r') as P:
        for info in P.readlines():
            morsegraph,morseset,param = info.replace('|',' ').split()
            fname_domgraph='{}dsgrn_domaingraph_{}.json'.format(jsonbasedir,unique_identifier)
            fileWrite(networkfile,fname_domgraph,'domaingraph json {}'.format(param))
            fname_morseset='{}dsgrn_morseset_{}.json'.format(jsonbasedir,unique_identifier)
            fileWrite(networkfile,fname_morseset,'analyze morseset {} {}'.format(morseset,param))
            try:
                patterns,matches=patternmatch.callPatternMatch(fname_morseset,fname_domgraph,fname_domcells,patternfile,resultsfile,writetofile=0,returnmatches=1,printtoscreen=printtoscreen,findallmatches=findallmatches)
                for pat,match in zip(patterns,matches):
                    if findallmatches:
                        R.write("Parameter: {}, Morse Graph: {}, Morse Set: {}, Pattern: {}, Results: {}".format(param,morsegraph,morseset,pat,match)+'\n')
                    else:
                        R.write("Parameter: {}, Morse Graph: {}, Morse Set: {}, Pattern: {}".format(param,morsegraph,morseset,pat)+'\n')
            except Exception as e:
                print 'Problem parameter is {}'.format(param)
                print traceback.format_exception_only(type(e),e)
                sys.stdout.flush()
            removeFiles([fname_domgraph,fname_morseset])
    removeFiles([fname_domcells,paramfile])

def fileWrite(networkfile,savefile,command):
    with open(savefile,'w',0) as f:
        subprocess.call(["dsgrn network {} {} ".format(networkfile,command)],shell=True,stdout=f)
    # subprocess.call(["dsgrn network {} domaingraph > {}".format(networkfile,fname_domcells)],shell=True)

def splitParamFile(paramfile,subparamfilestart,numparams,ncpus):
    numjobs,paramsperslice=calculateNumberofJobs(numparams,ncpus)
    with open(paramfile,'r') as apf:
        for n in range(numjobs):
            subparamfile=subparamfilestart+'_{:04d}.txt'.format(n)
            with open(subparamfile,'w',0) as subapf:
                for line in [apf.readline() for _ in range(paramsperslice)]:
                    if len(line):
                        subapf.write(line)
    return numjobs,paramsperslice

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

def parallelrun(job_server,numjobs,paramsperslice,subparamfilestart,allparamsfile,resultsfile,allresultsfile,ncpus,patternfile,networkfile,jsonbasedir,printtoscreen=0,findallmatches=0):
    jobs=[]
    allsubresultsfiles=[]
    for n in range(numjobs):
        unique_identifier='{:04d}'.format(n)
        subparamfile=subparamfilestart+'_'+unique_identifier+'.txt'
        subresultsfile=resultsfile+unique_identifier+'.txt'
        allsubresultsfiles.append(subresultsfile)        
        jobs.append(job_server.submit(patternSearch,(networkfile,jsonbasedir,unique_identifier,subparamfile,subresultsfile,patternfile,printtoscreen,findallmatches), depfuncs=(),modules = ("subprocess","patternmatch", "preprocess","fileparsers","walllabels","itertools","numpy","json","traceback","sys"),globals=globals()))
    print "All jobs starting."
    sys.stdout.flush()
    for job in jobs:
        job()
    job_server.destroy()
    print "All jobs ended."
    sys.stdout.flush()
    mergeFiles(allresultsfile,allsubresultsfiles)
    print "Results files merged."
    sys.stdout.flush()

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

def main_conley3_filesystem(patternsetter,getMorseGraphs,networkfilename="5D_2015_09_11",morsegraphselection="stableFCs",ncpus=1,printtoscreen=0,findallmatches=0):
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
    subparamfilestart=allparamsfile[:-4]
    numjobs,paramsperslice = splitParamFile(allparamsfile,subparamfilestart,numparams,ncpus)
    return [numjobs,paramsperslice,subparamfilestart,allparamsfile,resultsfile+'_results_',allresultsfile,ncpus,patternfile,networkfilebasedir+networkfilename+".txt",jsonbasedir,printtoscreen,findallmatches]

def main_local_filesystem(patternsetter,allparamsfile,networkfilename="5D_2015_09_11",morsegraphselection="stableFCs",ncpus=1,printtoscreen=0,findallmatches=0):
    # set patterns
    patternfile='/Users/bcummins/patternmatch_helper_files/patterns/patterns_'+networkfilename+'.txt'
    patternsetter(patternfile)
    # set file names and paths
    networkfilebasedir="/Users/bcummins/GIT/DSGRN/networks/"
    resultsbasedir='/Users/bcummins/patternmatch_helper_files/parameterresults/'
    resultsfile=resultsbasedir+networkfilename
    allresultsfile=resultsfile+'_'+morsegraphselection+'_allresults.txt'
    jsonbasedir='/Users/bcummins/patternmatch_helper_files/JSONfiles/'
    # get all parameters from all morse graphs
    with open(allparamsfile,'r') as apf:
        numparams=len(list(apf.readlines()))
    subparamfilestart=allparamsfile[:-4]
    numjobs,paramsperslice = splitParamFile(allparamsfile,subparamfilestart,numparams,ncpus)
    return [numjobs,paramsperslice,subparamfilestart,allparamsfile,resultsfile+'_results_',allresultsfile,ncpus,patternfile,networkfilebasedir+networkfilename+".txt",jsonbasedir,printtoscreen,findallmatches]

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
    with open(morsegraphfile,'w',0) as m:
        subprocess.call(['''sqlite3 /share/data/CHomP/Projects/DSGRN/DB/data/{}.db 'select MorseGraphIndex,Vertex from MorseGraphAnnotations where Label="FC" except select MorseGraphIndex,Source from MorseGraphEdges' '''.format(networkfile)],shell=True,stdout=m)

def selectAnyFC(networkfile,morsegraphfile):
    with open(morsegraphfile,'w',0) as m:
        subprocess.call(['''sqlite3 /share/data/CHomP/Projects/DSGRN/DB/data/{}.db 'select MorseGraphIndex,Vertex from MorseGraphAnnotations where Label="FC"' '''.format(networkfile)],shell=True,stdout=m)


if __name__=='__main__':
    # morsegraphselection="stableFCs"
    # getMorseGraphs=selectStableFC
    # networkfilename="5D_Cycle"
    # patternsetter=setPattern_5D_Cycle
    # networkfilename="5D_2015_09_11"
    # patternsetter=setPattern_Malaria_20hr_2015_09_11
    morsegraphselection="anyFCs"
    getMorseGraphs=selectAnyFC
    networkfilename="3D_Cycle"
    patternsetter=setPattern_3D_Cycle
    allparamsfile='/Users/bcummins/patternmatch_helper_files/parameterfiles/3D_Cycle_'+morsegraphselection+'_concatenatedparams.txt'
    ncpus=int(sys.argv[1])

    # listofargs=main_local_filesystem(patternsetter,allparamsfile,networkfilename,morsegraphselection,ncpus,printtoscreen=1,findallmatches=0)
    # patternSearch(listofargs[8],listofargs[9],'0',listofargs[3],listofargs[4],listofargs[7],listofargs[-2],listofargs[-1])

    listofargs=main_conley3_filesystem(patternsetter,getMorseGraphs,networkfilename,morsegraphselection,int(sys.argv[1]),printtoscreen=0,findallmatches=0)
    # job_server = pp.Server(ncpus=0,ppservers=("*",))
    job_server = pp.Server(ncpus=ncpus)
    # time.sleep(30)
    parallelrun(job_server,*listofargs)
