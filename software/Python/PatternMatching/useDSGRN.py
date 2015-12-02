from itertools import permutations, islice
import subprocess
import patternmatch
import pp,sys,time,os,glob,traceback
import fileparsers
from math import ceil

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

def patternSearch(morseset,patternfile='patterns.txt',networkfile="networks/5D_Model_B.txt",paramfile="5D_Model_B_FCParams.txt",resultsfile='results_5D_B.txt',jsonbasedir='/share/data/bcummins/JSONfiles/',printtoscreen=0,printparam=0,findallmatches=1,unique_identifier='0'):
    fname_domcells="{}dsgrn_domaincells_{}.json".format(jsonbasedir,unique_identifier)
    subprocess.call(["dsgrn network {} domaingraph > {}".format(networkfile,fname_domcells)],shell=True)
    R=open(resultsfile,'w',0)
    P=open(paramfile,'r')
    paramcount=1
    for param in P.readlines():
        param=param.split()[0]
        if printparam and paramcount%1000==0:
            print str(paramcount)+' parameters checked'
            sys.stdout.flush()
        paramcount+=1
        # shell call to dsgrn to produce dsgrn_output.json, which is the input for the pattern matcher
        fname_domgraph='{}dsgrn_domaingraph_{}.json'.format(jsonbasedir,unique_identifier)
        subprocess.call(["dsgrn network {} domaingraph json {} > {}".format(networkfile,int(param),fname_domgraph)],shell=True)
        fname_morseset='{}dsgrn_output_{}.json'.format(jsonbasedir,unique_identifier)
        subprocess.call(["dsgrn network {} analyze morseset {} {} > {}".format(networkfile,morseset,int(param),fname_morseset)],shell=True)
        try:
            patterns,matches=patternmatch.callPatternMatch(fname_morseset=fname_morseset,fname_domgraph=fname_domgraph,fname_domcells=fname_domcells,fname_patterns=patternfile,fname_results=resultsfile,writetofile=0,returnmatches=1,printtoscreen=printtoscreen,findallmatches=findallmatches)
            for pat,match in zip(patterns,matches):
                if findallmatches:
                    R.write("Parameter: {}, Morseset: {}, Pattern: {}, Results: {}".format(param,morseset,pat,match)+'\n')
                else:
                    R.write("Parameter: {}, Morseset: {}, Pattern: {}".format(param,morseset,pat)+'\n')
        except Exception as e:
            print 'Problem parameter is {}'.format(param)
            print traceback.format_exception_only(type(e), e) 
            sys.stdout.flush()
    R.close()
    P.close()

def splitParams(paramfile="5D_Malaria_2015_FCParams_MorseGraph565.txt",ncpus=1):
    pfile=open(paramfile,'r')
    paramlist=list(pfile.readlines())
    pfile.close()
    paramsperfile=int(ceil(float(len(paramlist))/ncpus))
    if paramsperfile==1:
        # don't split the file if there are fewer parameters than cpus
        # subprocess.call(["cp "+paramfile+" "+paramfile[:-4]+"_params_0000.txt"])
        # if there are fewer parameters than cpus, alter number of files to create
        ncpus=len(paramlist)
    for n in range(ncpus):
        f=open(paramfile[:-4]+'_params_{:04d}'.format(n)+'.txt','w')
        for param in paramlist[n*paramsperfile:min([(n+1)*paramsperfile,len(paramlist)])]:
            f.write(param)
        f.close()
    return ncpus

def mergeFiles(mergefile,splitfiles):
    m = open(mergefile,'a')
    for sfile in splitfiles:
        f=open(sfile,'r')
        for l in f.readlines():
            m.write(l)
        f.close()
        subprocess.call(['rm '+sfile],shell=True)
    m.close()

def cleanFiles(directory):
    for f in glob.glob(directory+'*'):
        os.remove(f)

def setPattern_Malaria_20hr_2015_09_11(patternfile='/share/data/bcummins/patterns_5D_2015_09_11.txt'):
    f=open(patternfile,'w')
    for s1 in permutations(['x3 max','x5 max','x1 max', 'x4 max']):
        patternstr1=', '.join(s1) + ', x2 max, '
        for s2 in permutations(['x1 min', 'x3 min','x5  min', 'x2 min','x4 min']):
            patternstr2=patternstr1 + ', '.join(s2) + ', ' + s1[0] + '\n'
            f.write(patternstr2)
    f.close()
    return patternfile

def setPattern_5D_Cycle(patternfile='/share/data/bcummins/patterns_5D_Cycle.txt'):
    f=open(patternfile,'w')
    f.write('X3 max, X4 max, X3 min, X4 min\n X3 max, X4 min, X3 min, X4 max')
    f.close()
    return patternfile

def setPattern_3D_Example(patternfile='patterns.txt'):
    with open(patternfile,'w') as f:
        f.write('Z min, X min, Y min, Z max, X max, Y max\n X max, Y max, Z max, X min, Y min, Z min\n X min, Y max, Z min, X max, Y min, Z max\n X max, Y min, Z max, X min, Y max, Z min')
    return patternfile

def setPattern_3D_Cycle(patternfile='patterns.txt'):
    with open(patternfile,'w') as f:
        f.write('X3 min, X1 min, X2 min, X3 max, X1 max, X2 max\n X1 max, X2 max, X3 max, X1 min, X2 min, X3 min\n X1 min, X2 max, X3 min, X1 max, X2 min, X3 max\n X1 max, X2 min, X3 max, X1 min, X2 max, X3 min')
    return patternfile

def parallelrun_on_conley3(morsegraph,morseset,patternfile,networkfile="/home/bcummins/DSGRN/networks/5D_2015_09_11.txt",parambasedir="/share/data/bcummins/parameterfiles/",paramfile="5D_2015_09_11_FCParams_MorseGraph565.txt",resultsbasedir="/share/data/bcummins/parameterresults/",jsonbasedir='/share/data/bcummins/JSONfiles/',printtoscreen=0,printparam=0,findallmatches=0,numnodes=1):
    ppservers = ("*",)
    job_server = pp.Server(ncpus=0,ppservers=ppservers)
    time.sleep(30)
    #if numservers:
     #   job_server=pp.Server(ppservers=numservers)
    #else:
        # tuple of all parallel python servers to connect with
        # ppservers = ()
        # Creates jobserver with automatically detected # of workers
        # job_server = pp.Server(ppservers=ppservers)
    # N = len(job_server.get_active_nodes())
    # print("Starting pp with " + str(N) + " workers.")
    # sys.stdout.flush()
    # split the parameter file into N chunks
    # N=splitParams(parambasedir+paramfile,N)
    # start the jobs
    jobs=[]
    allsubresultsfiles=[]
    paramname=paramfile.split('.')[0]
    parampath=parambasedir+paramname
    resultpath=resultsbasedir+paramname
    for i in range(numnodes):
        unique_identifier='{:05d}_{:02d}_{:04d}'.format(int(morsegraph),int(morseset),i)
        subparamfile=parampath+'_params_{:04d}'.format(i)+'.txt'
        subresultsfile=resultpath+'_results_{:04d}'.format(i)+'.txt'
        allsubresultsfiles.append(subresultsfile)
        jobs.append(job_server.submit( patternSearch,(morseset,patternfile,networkfile,subparamfile,subresultsfile,jsonbasedir,printtoscreen,printparam,findallmatches,unique_identifier), depfuncs=(),modules = ("subprocess","patternmatch", "pp", "preprocess","fileparsers","walllabels","itertools","numpy","json","sys","traceback"),globals=globals()))
    print "All jobs starting."
    sys.stdout.flush()
    for job in jobs:
        job()
    # job_server.print_stats()
    job_server.destroy()
    return allsubresultsfiles

def loopOverMorseGraphs(morsegraphfile,patternfile,networkfilebasedir="/home/bcummins/DSGRN/networks/",networkfilename="5D_Cycle.txt",resultsbasedir="/share/data/bcummins/parameterresults/",savefilename="5D_Cycle_StableFC_all_morse_graphs.txt",parambasedir="/share/data/bcummins/parameterfiles/",jsonbasedir='/share/data/bcummins/JSONfiles/',printtoscreen=0,printparam=0,findallmatches=0,numnodes=1):
    # parse morse graphs
    morse_graphs_and_sets=fileparsers.parseMorseGraphs(morsegraphfile)
    allresultsfile=resultsbasedir+savefilename
    # loop over morse graphs
    for (mgraph,mset) in morse_graphs_and_sets:
        paramname=networkfilename[:-4]+'_{:05d}.txt'.format(int(mgraph))
        paramfile=parambasedir+paramname
        subprocess.call(["sqlite3 /share/data/CHomP/Projects/DSGRN/DB/data/{}.db 'select ParameterIndex from Signatures where MorseGraphIndex={}' > {}".format(networkfilename[:-4],mgraph,paramfile)],shell=True)
        N=splitParams(paramfile,numnodes)
        for s in mset:
            allsubresultsfiles=parallelrun_on_conley3(mgraph,s,patternfile,networkfilebasedir+networkfilename,parambasedir,paramname,resultsbasedir,jsonbasedir,printtoscreen,printparam,findallmatches,numnodes)
            mergeFiles(allresultsfile,allsubresultsfiles)
            cleanFiles(jsonbasedir)
        cleanFiles(parambasedir)

def selectStableFC(networkfile,morsegraphfile):
    subprocess.call(['''sqlite3 /share/data/CHomP/Projects/DSGRN/DB/data/{}.db 'select MorseGraphIndex,Vertex from MorseGraphAnnotations where Label="FC" except select MorseGraphIndex,Source from MorseGraphEdges' > {}'''.format(networkfile,morsegraphfile)],shell=True)

def main_conley3_filesystem(networkfilename="5D_2015_09_11",patternsetter=setPattern_Malaria_20hr_2015_09_11,numnodes=1,getMorseGraphs=selectStableFC):
    # find all morse graphs with desired characteristic
    networkfilebasedir="/share/data/bcummins/DSGRN/networks/"
    morsegraphfile="/share/data/bcummins/"+networkfilename+'_stableFCs_listofmorsegraphs.txt'
    getMorseGraphs(networkfilename,morsegraphfile)
    # set patterns
    patternfile=patternsetter()
    # set other file names and paths
    parambasedir="/share/data/bcummins/parameterfiles/"
    resultsbasedir='/share/data/bcummins/parameterresults/'
    savefilename=networkfilename+'_stableFCs_allresults.txt'
    jsonbasedir='/share/data/bcummins/JSONfiles/'
    # call parallel runs for each Morse graph
    loopOverMorseGraphs(morsegraphfile,patternfile,networkfilebasedir,networkfilename+'.txt',resultsbasedir,savefilename,parambasedir,jsonbasedir,printtoscreen=0,printparam=0,findallmatches=0,numnodes=numnodes)

def patternSearch2(patternfile='patterns.txt',networkfile="networks/5D_Model_B.txt",paramfile="5D_Model_B_FCParams.txt",resultsfile='results_5D_B.txt',jsonbasedir='/share/data/bcummins/JSONfiles/',printtoscreen=0,printparam=0,findallmatches=1,unique_identifier='0'):
    # get domain cells for the network via shell call to dsgrn
    fname_domcells="{}dsgrn_domaincells_{}.json".format(jsonbasedir,unique_identifier)
    subprocess.call(["dsgrn network {} domaingraph > {}".format(networkfile,fname_domcells)],shell=True)
    # call the pattern matcher for each parameter and record the results
    with open(resultsfile,'w',0) as R, open(paramfile,'r') as P:
        paramcount=1
        for info in P.readlines():
            morsegraph,morseset,param = info.split('|')
            if printparam and paramcount%1000==0:
                print str(paramcount)+' parameters checked'
                sys.stdout.flush()
            paramcount+=1
            # shell call to dsgrn to produce json files as input for the pattern matcher
            fname_domgraph='{}dsgrn_domaingraph_{}.json'.format(jsonbasedir,unique_identifier)
            subprocess.call(["dsgrn network {} domaingraph json {} > {}".format(networkfile,param,fname_domgraph)],shell=True)
            fname_morseset='{}dsgrn_output_{}.json'.format(jsonbasedir,unique_identifier)
            subprocess.call(["dsgrn network {} analyze morseset {} {} > {}".format(networkfile,morseset,param,fname_morseset)],shell=True)
            try:
                patterns,matches=patternmatch.callPatternMatch(fname_morseset=fname_morseset,fname_domgraph=fname_domgraph,fname_domcells=fname_domcells,fname_patterns=patternfile,fname_results=resultsfile,writetofile=0,returnmatches=1,printtoscreen=printtoscreen,findallmatches=findallmatches)
                for pat,match in zip(patterns,matches):
                    if findallmatches:
                        R.write("Parameter: {}, Morse Graph: {}, Morse Set: {}, Pattern: {}, Results: {}".format(param,morsegraph,morseset,pat,match)+'\n')
                    else:
                        R.write("Parameter: {}, Morse Graph: {}, Morseset: {}, Pattern: {}".format(param,morsegraph,morseset,pat)+'\n')
            except Exception as e:
                print 'Problem parameter is {}'.format(param)
                raise
                # print traceback.format_exception_only(type(e), e) 
                # sys.stdout.flush()

def concatenateParams(allparamsfile,morse_graphs_and_sets):
    numparams=0
    with open(allparamsfile,'w') as apf:
        for (mgraph,mset) in morse_graphs_and_sets:
            subprocess.call(["sqlite3 /share/data/CHomP/Projects/DSGRN/DB/data/{}.db 'select ParameterIndex from Signatures where MorseGraphIndex={}' > param_temp.txt".format(networkfilename,mgraph)],shell=True)
            with open('param_temp.txt','r') as p:
                for ms in mset:
                    for param in p.readlines():
                        numparams+=1
                        apf.write('{}|{}|{}'.format(mgraph,ms,param))
    return numparams

def splitParams2(paramfile,numparams,ncpus,allparamsfile):
    paramsperfile=int(ceil(float(numparams)/ncpus))
    new_ncpus=0
    with open(allparamsfile,'r') as apf2:
        for n in range(ncpus):
            if n*paramsperfile < numparams:
                new_ncpus+=1
                head = islice(apf2,paramsperfile)
                with open(paramfile+'{:04d}'.format(n)+'.txt','w') as f:
                    for h in head:
                        f.write(h)
    return new_ncpus

def startServers_conley3():
    job_server = pp.Server(ncpus=0,ppservers=("*",))
    print "Starting pp with", job_server.get_ncpus(), "workers"
    sys.stdout.flush()
    time.sleep(30)
    return job_server

def startServers_local():
    job_server = pp.Server(ppservers=("*",))
    print "Starting pp with", job_server.get_ncpus(), "workers"
    return job_server

def parallelrun(paramfile,resultsfile,ncpus,patternfile,networkfile,jsonbasedir,printtoscreen=0,printparam=0,findallmatches=0,startservers=startServers_conley3):
    job_server = startservers()
    jobs=[]
    allsubresultsfiles=[]
    for n in range(ncpus):
        unique_identifier='{:04d}'.format(n)
        subparamfile=paramfile+unique_identifier+'.txt'
        subresultsfile=resultsfile+unique_identifier+'.txt'
        allsubresultsfiles.append(subresultsfile)
        jobs.append(job_server.submit( patternSearch2,(patternfile,networkfile,subparamfile,subresultsfile,jsonbasedir,printtoscreen,printparam,findallmatches,unique_identifier), depfuncs=(),modules = ("subprocess","patternmatch", "preprocess","fileparsers","walllabels","itertools","numpy","json","traceback","sys"),globals=globals()))
    print "All jobs starting."
    sys.stdout.flush()
    for job in jobs:
        job()
        job_server.print_stats()
        sys.stdout.flush()
    job_server.destroy()
    print "All jobs ended."
    sys.stdout.flush()
    return allsubresultsfiles


def main_conley3_filesystem_allparameters(networkfilename="5D_2015_09_11",morsegraphselection="stableFCs",patternsetter=setPattern_Malaria_20hr_2015_09_11,ncpus=1,getMorseGraphs=selectStableFC,printtoscreen=0,printparam=0,findallmatches=0):
    # find all morse graphs with desired characteristic
    networkfilebasedir="/share/data/bcummins/DSGRN/networks/"
    morsegraphfile="/share/data/bcummins/"+networkfilename+'_'+morsegraphselection+'_listofmorsegraphs.txt'
    getMorseGraphs(networkfilename,morsegraphfile)
    # set patterns
    patternfile=patternsetter('/share/data/bcummins/patterns_'+networkfilename+'.txt')
    # set other file names and paths
    parambasedir="/share/data/bcummins/parameterfiles/"
    paramfile=parambasedir+networkfilename
    resultsbasedir='/share/data/bcummins/parameterresults/'
    resultsfile=resultsbasedir+networkfilename
    allresultsfile=resultsfile+'_'+morsegraphselection+'_allresults.txt'
    jsonbasedir='/share/data/bcummins/JSONfiles/'
    # parse morse graphs
    morse_graphs_and_sets=fileparsers.parseMorseGraphs(morsegraphfile)
    # get all parameters from all morse graphs
    allparamsfile=paramfile+'_concatenatedparams.txt'
    numparams=concatenateParams(allparamsfile,morse_graphs_and_sets)
    # split parameter file into one for each cpu
    ncpus=splitParams2(paramfile+'_params_',numparams,ncpus,allparamsfile)
    # run parallel process
    allsubresultsfiles=parallelrun(paramfile+'_params_',resultsfile+'_results_',ncpus,patternfile,networkfilebasedir+networkfilename+".txt",jsonbasedir,printtoscreen,printparam,findallmatches)
    # concatenate results
    mergeFiles(allresultsfile,allsubresultsfiles)
    print "Results files merged."
    sys.stdout.flush()


def main_local_filesystem_allparameters(networkfilename="5D_2015_09_11",morsegraphselection="stableFCs",allparamsfile="5D_2015_09_11_stableFCs_listofmorsegraphs.txt",patternsetter=setPattern_Malaria_20hr_2015_09_11,ncpus=1,printtoscreen=0,printparam=0,findallmatches=0):
    # set patterns
    patternfile=patternsetter('/Users/bcummins/patternmatch_helper_files/patterns_'+networkfilename+'.txt')
    # set file names and paths
    networkfilebasedir="/Users/bcummins/GIT/DSGRN/networks/"
    parambasedir="/Users/bcummins/patternmatch_helper_files/parameterfiles/"
    paramfile=parambasedir+networkfilename
    resultsbasedir='/Users/bcummins/patternmatch_helper_files/parameterresults/'
    resultsfile=resultsbasedir+networkfilename
    allresultsfile=resultsfile+'_'+morsegraphselection+'_allresults.txt'
    jsonbasedir='/Users/bcummins/patternmatch_helper_files/JSONfiles/'
    # count number of parameters
    numparams=0
    with open(allparamsfile,'r') as apf:
        for _ in apf.readlines():
            numparams+=1
    # split parameter file into one for each cpu
    ncpus=splitParams2(paramfile+'_params_',numparams,ncpus,allparamsfile)
    # run parallel process
    allsubresultsfiles=parallelrun(paramfile+'_params_',resultsfile+'_results_',ncpus,patternfile,networkfilebasedir+networkfilename+".txt",jsonbasedir,printtoscreen,printparam,findallmatches,startservers=startServers_local)
    # concatenate results
    mergeFiles(allresultsfile,allsubresultsfiles)
    print "Results files merged."
    sys.stdout.flush()


if __name__=='__main__':
    morsegraphselection="stableFCs"
    getMorseGraphs=selectStableFC
    # networkfilename="5D_Cycle"
    # patternsetter=setPattern_5D_Cycle
    # networkfilename="5D_2015_09_11"
    # patternsetter=setPattern_Malaria_20hr_2015_09_11
    # main_conley3_filesystem(networkfilename,morsegraphselection,patternsetter,int(sys.argv[1]),getMorseGraphs)
    networkfilename="3D_Cycle"
    patternsetter=setPattern_3D_Cycle
    allparamsfile="/Users/bcummins/patternmatch_helper_files/3D_Cycle_concatenatedparams.txt"
    main_local_filesystem_allparameters(networkfilename,morsegraphselection,allparamsfile,patternsetter,ncpus=4,printtoscreen=1)

    # patternSearch2(patternfile='patterns.txt',networkfile="networks/5D_Model_B.txt",paramfile="5D_Model_B_FCParams.txt",resultsfile='results_5D_B.txt',jsonbasedir='/share/data/bcummins/JSONfiles/',printtoscreen=0,printparam=0,findallmatches=1,unique_identifier='0')
