# The MIT License (MIT)

# Copyright (c) 2015 Breschine Cummins

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

import malaria.patternsnatcher as patternsnatch
import itertools
import subprocess
import patternmatch
import sys,time,traceback
import pp,os,json,glob
import fileparsers
from math import ceil

# Call this module as python useDSGRN.py

# Given a Morse graph number MGN for a database DATABASEFILE (.db), do the following database searches:

# The following gives the list of parameters in the file PARAMFILE (.txt)
# sqlite3 /share/data/CHomP/Projects/DSGRN/DB/data/DATABASEFILE.db 'select ParameterIndex from Signatures where MorseGraphIndex=MGN' > ./PARAMFILE

# The following finds the Morse set number:
# sqlite3 /share/data/CHomP/Projects/DSGRN/DB/data/DATABASEFILE.db 'select * from MorseGraphAnnotations where MorseGraphIndex=MGN'


# Count the number of parameters in all stable FCs:
# sqlite3 /share/data/CHomP/Projects/DSGRN/DB/data/DATABASEFILE.db 'select count(*) from Signatures natural join (select distinct(MorseGraphIndex) from (select MorseGraphIndex,Vertex from MorseGraphAnnotations where Label="FC" except select MorseGraphIndex,Source from MorseGraphEdges))'



# def patternSearch(networkfile,paramfile,resultsfile,patternstr,printtoscreen,findallmatches):
#     domaincells_jsonstr=getJSONstring(networkfile,['domaingraph'])
#     results_list=[]
#     with open(paramfile,'r') as P:
#         for info in P.readlines():
#             morsegraph,morseset,param = info.replace('|',' ').split()
#             domaingraph_jsonstr=getJSONstring(networkfile,['domaingraph', 'json',param])
#             morseset_jsonstr=getJSONstring(networkfile,['analyze', 'morseset', morseset, param])
#             try:
#                 patterns,matches=patternmatch.callPatternMatch(morseset_jsonstr,domaingraph_jsonstr,domaincells_jsonstr,patternstr,resultsfile,writetofile=0,returnmatches=1,printtoscreen=printtoscreen,findallmatches=findallmatches)
#                 for pat,match in zip(patterns,matches):
#                     if findallmatches:
#                         results_list.append("Parameter: {}, Morse Graph: {}, Morse Set: {}, Pattern: {}, Results: {}".format(param,morsegraph,morseset,pat,match)+'\n')
#                     else:
#                         results_list.append("Parameter: {}, Morse Graph: {}, Morse Set: {}, Pattern: {}".format(param,morsegraph,morseset,pat)+'\n')
#             except Exception as e:
#                 print 'Problem: Morse graph {}, Morse set {}, Parameter {}'.format(morsegraph,morseset,param)
#                 print traceback.format_exception_only(type(e),e)
#                 sys.stdout.flush()
#     with open(resultsfile,'w',0) as R:
#         for line in results_list:
#             R.write(line)
#     removeFiles([paramfile])

def patternSearch_paramlist(networkfile,paramlist,resultsfile,patternstr,printtoscreen,findallmatches):
    domaincells_jsonstr=getJSONstring(networkfile,['domaingraph'])
    results_list=[]
    for info in paramlist:
        morsegraph,morseset,param = info.replace('|',' ').split()
        domaingraph_jsonstr=getJSONstring(networkfile,['domaingraph', 'json',param])
        morseset_jsonstr=getJSONstring(networkfile,['analyze', 'morseset', morseset, param])
        try:
            patterns,matches=patternmatch.callPatternMatch(morseset_jsonstr,domaingraph_jsonstr,domaincells_jsonstr,patternstr,resultsfile,writetofile=0,returnmatches=1,printtoscreen=printtoscreen,findallmatches=findallmatches)
            for pat,match in zip(patterns,matches):
                if findallmatches:
                    results_list.append("Parameter: {}, Morse Graph: {}, Morse Set: {}, Pattern: {}, Results: {}".format(param,morsegraph,morseset,pat,match)+'\n')
                else:
                    results_list.append("Parameter: {}, Morse Graph: {}, Morse Set: {}, Pattern: {}".format(param,morsegraph,morseset,pat)+'\n')
        except Exception as e:
            print 'Problem parameter is {}'.format(param)
            print traceback.format_exception_only(type(e),e)
            sys.stdout.flush()
    with open(resultsfile,'w',0) as R:
        for line in results_list:
            R.write(line)

def getJSONstring(networkfile,command=['analyze', 'morseset', '0', '0']):
    return subprocess.check_output(['dsgrn','network', networkfile]+command,shell=False)

# def splitParamFile(paramfile,subparamfilestart,numparams,paramsperslice):
#     numjobs=int(ceil(float(numparams)/paramsperslice))
#     with open(paramfile,'r') as apf:
#         for n in range(numjobs):
#             subparamfile=subparamfilestart+'_{:04d}.txt'.format(n)
#             with open(subparamfile,'w',0) as subapf:
#                 for line in [apf.readline() for _ in range(paramsperslice)]:
#                     if len(line):
#                         subapf.write(line)
#     return numjobs

def removeFiles(listoffiles):
    for f in listoffiles:
        try:
            subprocess.call(['rm '+f],shell=True)  
        except:
            pass         

def mergeFiles(mergefile,splitfiles):
    # splitfiles is a directory list iterator, glob.iglob
    count=0
    with open(mergefile,'w') as m:
        for sfile in splitfiles:
            with open(sfile,'r') as f:
                for l in f.readlines():
                    m.write(l) 
            count += 1           
            removeFiles([sfile])
    print '{} results files merged.'.format(count)
    sys.stdout.flush()


def parallelrun_paramlist(job_server,paramsperslice,allparamsfile,resultsfile,allresultsfile,patternstr,networkfile,printtoscreen=0,findallmatches=0):
    jobs=[]
    # allsubresultsfiles=[]
    with open(allparamsfile,'r') as apf:
        def get_lines():
            # construct a list containing paramsperslice parameters
            # the file object is a generator, so large files don't slow the process
            count=0
            paramlist=[]
            for line in apf:
                paramlist.append(line)
                count+=1
                if count==paramsperslice:
                    break
            return paramlist
        paramlist=get_lines()
        identity=0
        while paramlist:
            unique_identifier='{:04d}'.format(identity)
            identity+=1
            subresultsfile=resultsfile+unique_identifier+'.txt'
            # allsubresultsfiles.append(subresultsfile)        
            jobs.append(job_server.submit(patternSearch_paramlist,(networkfile,paramlist,subresultsfile,patternstr,printtoscreen,findallmatches), depfuncs=(),modules = ("subprocess","patternmatch", "preprocess","fileparsers","walllabels","itertools","numpy","json","traceback","sys"),globals=globals()))
            paramlist=get_lines()
    print "{} jobs starting.".format(identity)
    sys.stdout.flush()
    for job in jobs:
        job()
    job_server.destroy()
    print "All jobs ended."
    sys.stdout.flush()
    mergeFiles(allresultsfile,glob.iglob(resultsfile+"*"))
    sys.stdout.flush()

# def parallelrun(job_server,numjobs,subparamfilestart,resultsfile,allresultsfile,patternstr,networkfile,printtoscreen=0,findallmatches=0):
#     jobs=[]
#     allsubresultsfiles=[]
#     for n in range(numjobs):
#         unique_identifier='{:04d}'.format(n)
#         subparamfile=subparamfilestart+'_'+unique_identifier+'.txt'
#         subresultsfile=resultsfile+unique_identifier+'.txt'
#         allsubresultsfiles.append(subresultsfile)        
#         jobs.append(job_server.submit(patternSearch,(networkfile,subparamfile,subresultsfile,patternstr,printtoscreen,findallmatches), depfuncs=(),modules = ("subprocess","patternmatch", "preprocess","fileparsers","walllabels","itertools","numpy","json","traceback","sys"),globals=globals()))
#     print "{} jobs starting.".format(numjobs)
#     sys.stdout.flush()
#     for job in jobs:
#         job()
#     job_server.destroy()
#     print "All jobs ended."
#     sys.stdout.flush()
#     mergeFiles(allresultsfile,resultsfile)
#     print "Results files merged."
#     sys.stdout.flush()

def initServer():
    secret = os.environ["PPSERVERSECRET"]
    ppservers = ("*",)
    job_server = pp.Server(ncpus=0,ppservers=ppservers,secret=secret)
    time.sleep(30)
    N = sum(job_server.get_active_nodes().values())
    print job_server.get_active_nodes()
    print "There are", N, " cores in total."
    return job_server


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

# def main_conley3_filesystem(patternsetter,getMorseGraphs,networkfilename="5D_2015_09_11",morsegraphselection="stableFCs",morsegraph=0,morseset=0,paramsperslice=500,printtoscreen=0,findallmatches=0):
#     # find all morse graphs with desired characteristic
#     networkfilebasedir="/share/data/bcummins/DSGRN/networks/"
#     morsegraphfile="/share/data/bcummins/morsegraphs/"+networkfilename+'_'+morsegraphselection+'_listofmorsegraphs.txt'
#     getMorseGraphs(networkfilename,morsegraphfile,morsegraph,morseset)
#     # set patterns
#     patternstr=patternsetter()
#     # set other file names and paths
#     allparamsfile="/share/data/bcummins/parameterfiles/"+networkfilename+'_'+morsegraphselection+'_concatenatedparams.txt'
#     resultsbasedir='/share/data/bcummins/parameterresults/'
#     resultsfile=resultsbasedir+networkfilename
#     allresultsfile=resultsfile+'_'+morsegraphselection+'_allresults.txt'
#     # parse morse graphs
#     morse_graphs_and_sets=fileparsers.parseMorseGraphs(morsegraphfile)
#     # get all parameters from all morse graphs
#     numparams=concatenateParams(allparamsfile,morse_graphs_and_sets)
#     subparamfilestart=allparamsfile[:-4]
#     numjobs = splitParamFile(allparamsfile,subparamfilestart,numparams,paramsperslice)
#     return [numjobs,subparamfilestart,resultsfile+'_results_',allresultsfile,patternstr,networkfilebasedir+networkfilename+".txt",printtoscreen,findallmatches]

def main_conley3_filesystem_paramlist(patternsetter,getMorseGraphs,networkfilename="5D_2015_09_11",morsegraphselection="stableFCs",morsegraph=0,morseset=0,printtoscreen=0,findallmatches=0):
    # find all morse graphs with desired characteristic
    networkfilebasedir="/share/data/bcummins/DSGRN/networks/"
    morsegraphfile="/share/data/bcummins/morsegraphs/"+networkfilename+'_'+morsegraphselection+'_listofmorsegraphs.txt'
    getMorseGraphs(networkfilename,morsegraphfile,morsegraph,morseset)
    # set patterns
    patternstr=patternsetter()
    # set other file names and paths
    allparamsfile="/share/data/bcummins/parameterfiles/"+networkfilename+'_'+morsegraphselection+'_concatenatedparams.txt'
    resultsbasedir='/share/data/bcummins/parameterresults/'
    resultsfile=resultsbasedir+networkfilename
    allresultsbasedir='/share/data/bcummins/parameterresults_archive/'
    allresultsfile=allresultsbasedir+networkfilename+'_'+morsegraphselection+'_allresults.txt'
    # parse morse graphs
    morse_graphs_and_sets=fileparsers.parseMorseGraphs(morsegraphfile)
    # get all parameters from all morse graphs
    numparams=concatenateParams(allparamsfile,morse_graphs_and_sets)
    return [allparamsfile,resultsfile+'_results_',allresultsfile,patternstr,networkfilebasedir+networkfilename+".txt",printtoscreen,findallmatches]

# def main_local_filesystem(patternsetter,allparamsfile,networkfilename="5D_2015_09_11",morsegraphselection="stableFCs",paramsperslice=500,printtoscreen=0,findallmatches=0):
#     # set patterns
#     patternstr=patternsetter()
#     # set file names and paths
#     networkfilebasedir="/Users/bcummins/GIT/DSGRN/networks/"
#     resultsbasedir='/Users/bcummins/patternmatch_helper_files/parameterresults/'
#     resultsfile=resultsbasedir+networkfilename
#     allresultsbasedir='/Users/bcummins/patternmatch_helper_files/parameterresults_archive/'
#     allresultsfile=allresultsbasedir+networkfilename+'_'+morsegraphselection+'_allresults.txt'
#     # get all parameters from all morse graphs
#     with open(allparamsfile,'r') as apf:
#         numparams=len(list(apf.readlines()))
#     subparamfilestart=allparamsfile[:-4]
#     numjobs = splitParamFile(allparamsfile,subparamfilestart,numparams,paramsperslice)
#     return [numjobs,subparamfilestart,resultsfile+'_results_',allresultsfile,patternstr,networkfilebasedir+networkfilename+".txt",printtoscreen,findallmatches]

def main_local_filesystem_paramlist(patternsetter,allparamsfile,networkfilename="5D_2015_09_11",morsegraphselection="stableFCs",printtoscreen=0,findallmatches=0):
    # set patterns
    patternstr=patternsetter()
    # set file names and paths
    networkfilebasedir="/Users/bcummins/GIT/DSGRN/networks/"
    resultsbasedir='/Users/bcummins/patternmatch_helper_files/parameterresults/'
    resultsfile=resultsbasedir+networkfilename
    allresultsbasedir='/Users/bcummins/patternmatch_helper_files/parameterresults_archive/'
    allresultsfile=allresultsbasedir+networkfilename+'_'+morsegraphselection+'_allresults.txt'
    return [allparamsfile,resultsfile+'_results_',allresultsfile,patternstr,networkfilebasedir+networkfilename+".txt",printtoscreen,findallmatches]

def setPattern_Malaria_20hr_2015_09_11():
    patternstr=[]
    for s1 in itertools.permutations(['x3 max','x5 max','x1 max', 'x4 max']):
        patternstr1=', '.join(s1) + ', x2 max, '
        for s2 in itertools.permutations(['x1 min', 'x3 min','x5 min', 'x2 min','x4 min']):
            patternstr2=patternstr1 + ', '.join(s2) + ', ' + s1[0] + '\n'
            patternstr.append(patternstr2)
    return ' '.join(patternstr)
            
def setPattern_Malaria_20hr_2016_01_05():
    patternstr=[]
    for s1 in itertools.permutations(['199 max','177 max','72 max', '204 max']):
        patternstr1=', '.join(s1) + ', 77 max, '
        for s2 in itertools.permutations(['72 min', '199 min','177 min', '77 min','204 min']):
            patternstr2=patternstr1 + ', '.join(s2) + ', ' + s1[0] + '\n'
            patternstr.append(patternstr2)
    return ' '.join(patternstr)
            
def setPattern_Malaria_20hr_2016_01_05_move77min():
    patternstr=[]
    for s1 in itertools.permutations(['199 max','177 max','72 max', '204 max', '77 min']):
        patternstr1=', '.join(s1) + ', 77 max, '
        for s2 in itertools.permutations(['72 min', '199 min','177 min','204 min']):
            patternstr2=patternstr1 + ', '.join(s2) + ', ' + s1[0] + '\n'
            patternstr.append(patternstr2)
    return ' '.join(patternstr)
            
def setPattern_Malaria_20hr_2016_01_05_patternsnatch():
    names = ['PF3D7_0504700','PF3D7_0506700','PF3D7_0818700','PF3D7_0919000','PF3D7_0925700'] 
    aliases =  ['72', '77', '177', '199', '204']
    savename = 'patterns_43genes_5Dnode.txt'
    patternsnatch.constructPatterns(names,savename,patternsnatch.dataStruct43Genes)
    patternstr = ''
    with open(savename,'r') as s:
        for line in s:
            for a,n in zip(aliases,names):
                line = line.replace(n,a)
            patternstr+=line
    return patternstr
    
    patternstr=[]
    for s1 in itertools.permutations(['x3 max','x5 max','x1 max', 'x4 max']):
        patternstr1=', '.join(s1) + ', x2 max, '
        for s2 in itertools.permutations(['x1 min', 'x3 min','x5  min', 'x2 min','x4 min']):
            patternstr2=patternstr1 + ', '.join(s2) + ', ' + s1[0] + '\n'
            patternstr.append(patternstr2)
    return ' '.join(patternstr)
            
def setPattern_5D_Cycle():
    return 'X3 max, X4 max, X3 min, X4 min\n X3 max, X4 min, X3 min, X4 max'

def setPattern_3D_Example():
    return 'Z min, X min, Y min, Z max, X max, Y max\n X max, Y max, Z max, X min, Y min, Z min\n X min, Y max, Z min, X max, Y min, Z max\n X max, Y min, Z max, X min, Y max, Z min'

def setPattern_3D_Cycle():
    return 'X3 min, X1 min, X2 min, X3 max, X1 max, X2 max\n X1 max, X2 max, X3 max, X1 min, X2 min, X3 min\n X1 min, X2 max, X3 min, X1 max, X2 min, X3 max\n X1 max, X2 min, X3 max, X1 min, X2 max, X3 min \n X1 max, X3 max, X1 min, X3 min\n X1 max, X3 max, X2 max, X1 min, X3 min, X2 min'

def selectStableFC(networkfile,morsegraphfile,junk1,junk2):
    with open(morsegraphfile,'w',0) as m:
        subprocess.call(['''sqlite3 /share/data/CHomP/Projects/DSGRN/DB/data/{}.db 'select MorseGraphIndex,Vertex from MorseGraphAnnotations where Label="FC" except select MorseGraphIndex,Source from MorseGraphEdges' '''.format(networkfile)],shell=True,stdout=m)

def selectUnstableFC(networkfile,morsegraphfile,junk1,junk2):
    with open(morsegraphfile,'w',0) as m:
        subprocess.call(['''sqlite3 /share/data/CHomP/Projects/DSGRN/DB/data/{}.db 'select MorseGraphIndex,Vertex from MorseGraphAnnotations where Label="FC" INTERSECT select MorseGraphIndex,Source from MorseGraphEdges' '''.format(networkfile)],shell=True,stdout=m)

def selectAnyFC(networkfile,morsegraphfile,junk1,junk2):
    with open(morsegraphfile,'w',0) as m:
        subprocess.call(['''sqlite3 /share/data/CHomP/Projects/DSGRN/DB/data/{}.db 'select MorseGraphIndex,Vertex from MorseGraphAnnotations where Label="FC"' '''.format(networkfile)],shell=True,stdout=m)

def selectOneMorseGraph(networkfile,morsegraphfile,morsegraph,morseset):
    with open(morsegraphfile,'w',0) as m:
        m.write(str(morsegraph)+'|'+str(morseset))



if __name__=='__main__':
    # # Select which network to analyze
    # morsegraphselection="stableFCs"
    getMorseGraphs=selectStableFC
    # networkfilename="5D_Cycle"
    # patternsetter=setPattern_5D_Cycle
    # getMorseGraphs=selectOneMorseGraph
    # morsegraph=38564
    # morseset=0
    # morsegraphselection="MG38564FC"
    # networkfilename="5D_2015_09_11"
    # patternsetter=setPattern_Malaria_20hr_2015_09_11
    # networkfilename="5D_2016_01_05_C"
    # patternsetter=setPattern_Malaria_20hr_2016_01_05
    patternsetter = setPattern_Malaria_20hr_2016_01_05
    networkfilename="5D_2016_01_28_essential"    
    morsegraphselection="stableFCs"
    # networkfilename="6D_2016_01_29"    
    # morsegraphselection="MG4618_onebadparameter"
    # morsegraph=565
    # morseset=0
    # morsegraph=1298
    # morseset=2
    # morsegraph=4618
    # morseset=1
    # morsegraphselection="stableFCs"
    morsegraph=None
    morseset=None
    # morsegraph=1976
    # morseset=0
    # morsegraphselection="MG1976"
    # getMorseGraphs=selectOneMorseGraph
    # morsegraphselection="anyFCs"
    # getMorseGraphs=selectAnyFC
    # networkfilename="3D_Cycle"
    # patternsetter=setPattern_3D_Cycle
    paramsperslice=100
    # paramsperslice=5

    # # Run on local file system 
    # networkfilename="5D_2015_09_11"
    # patternsetter=setPattern_Malaria_20hr_2015_09_11
    # morsegraphselection="MG1298"
    # allparamsfile='/Users/bcummins/patternmatch_helper_files/parameterfiles/5D_2015_09_11_MG1298_concatenatedparams.txt'
    # with open(allparamsfile,'w') as ap:
    #     # ap.write('565|0|1196132\n565|0|1203690\n565|0|2654015\n565|0|6046050')
    #     ap.write('1298|2|3736292')
    #     # ap.write('4618|1|6079889')
    # # listofargs=main_local_filesystem(patternsetter,allparamsfile,networkfilename,morsegraphselection,paramsperslice,printtoscreen=0,findallmatches=0)
    # listofargs=main_local_filesystem_paramlist(patternsetter,allparamsfile,networkfilename,morsegraphselection,printtoscreen=0,findallmatches=0)
    # # paramfile=listofargs[1]+'_0000.txt'
    # # patternSearch_paramlist(listofargs[5],paramfile,listofargs[2],listofargs[4],0,0)
    # job_server = pp.Server(ppservers=("*",))
    # N = sum(job_server.get_active_nodes().values())
    # print "There are", N, " cores in total."
    # parallelrun_paramlist(job_server,paramsperslice,*listofargs)

    # Run on conley3 
    listofargs=main_conley3_filesystem_paramlist(patternsetter,getMorseGraphs,networkfilename,morsegraphselection,morsegraph,morseset,printtoscreen=0,findallmatches=0)
    job_server=initServer()
    parallelrun_paramlist(job_server,paramsperslice,*listofargs)


