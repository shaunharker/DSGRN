from itertools import permutations
import subprocess
import patternmatch
import pp

# Given a Morse graph number MGN for a database DATABASEFILE (.db), do the following database searches:
# 
# The following gives the list of parameters in the file PARAMFILE (.txt)
# sqlite3 /share/data/CHomP/Projects/DSGRN/DB/data/DATABASEFILE 'select ParameterIndex from Signatures where MorseGraphIndex=MGN' > ./PARAMFILE
#
# The following finds the Morse set number:
# sqlite3 /share/data/CHomP/Projects/DSGRN/DB/data/DATABASEFILE 'select * from MorseGraphAnnotations where MorseGraphIndex=MGN'
#
# The following is a search for stable FCs:
# sqlite3 /share/data/CHomP/Projects/DSGRN/DB/data/DATABASEFILE 'select MorseGraphIndex,Vertex from MorseGraphAnnotations where Label="FC" except select MorseGraphIndex,Source from MorseGraphEdges'
#
# So far the Morse set number is entered by hand, but I should write a parser eventually.

def patternSearch(morseset=0,networkfile="networks/5D_Model_B.txt",paramfile="5D_Model_B_FCParams.txt",resultsfile='results_5D_B.txt',printtoscreen=0,printparam=0,findallmatches=1,unique_identifier=0):
    subprocess.call(["dsgrn network {} domaingraph > dsgrn_domaincells_{:04d}.json".format(networkfile,unique_identifier)],shell=True)
    R=open(resultsfile,'w',0)
    P=open(paramfile,'r')
    paramcount=1
    for param in P.readlines():
        param=param.split()[0]
        if printparam and paramcount%1000==0:
            print str(paramcount)+' parameters checked'
        paramcount+=1
        # shell call to dsgrn to produce dsgrn_output.json, which is the input for the pattern matcher
        subprocess.call(["dsgrn network {} domaingraph json {} > dsgrn_domaingraph_{:04d}.json".format(networkfile,int(param),unique_identifier)],shell=True)
        subprocess.call(["dsgrn network {} analyze morseset {} {} >dsgrn_output_{:04d}.json".format(networkfile,morseset,int(param),unique_identifier)],shell=True)
        try:
            patterns,matches=patternmatch.callPatternMatch(fname_morseset='dsgrn_output_{:04d}.json'.format(unique_identifier),fname_domgraph='dsgrn_domaingraph_{:04d}.json'.format(unique_identifier),fname_domcells='dsgrn_domaincells_{:04d}.json'.format(unique_identifier),fname_patterns='patterns.txt',fname_results=resultsfile,writetofile=0,returnmatches=1,printtoscreen=printtoscreen,findallmatches=findallmatches)
        except ValueError:
            print 'Problem parameter is {}'.format(param)
            raise
        for pat,match in zip(patterns,matches):
            if findallmatches:
                R.write("Parameter: {}, Morseset: {}, Pattern: {}, Results: {}".format(param,morseset,pat,match)+'\n')
            else:
                R.write("Parameter: {}, Morseset: {}, Pattern: {}".format(param,morseset,pat)+'\n')
    R.close()
    P.close()

def patternSearchSingle(parameter,morseset=0,specfile="networks/5D_Model_B.txt",resultsfile='results.txt',printtoscreen=0):
    R=open(resultsfile,'w',0)
    # shell call to dsgrn to produce dsgrn_output.json, which is the input for the pattern matcher
    subprocess.call(["dsgrn network {} domaingraph > dsgrn_domaincells.json".format(specfile)],shell=True)
    subprocess.call(["dsgrn network {} domaingraph json {} > dsgrn_domaingraph.json".format(specfile,int(parameter))],shell=True)
    subprocess.call(["dsgrn network {} analyze morseset {} {} >dsgrn_output.json".format(specfile,morseset,parameter)],shell=True)
    try:
        patterns,matches=patternmatch.callPatternMatch(writetofile=0,returnmatches=1,printtoscreen=printtoscreen)
    except ValueError:
        print 'Problem parameter is {}'.format(parameter)
        raise
    for pat,match in zip(patterns,matches):
        R.write("Parameter: {}, Morseset: {}".format(parameter,morseset)+'\n')
        R.write("Pattern: {}".format(pat)+'\n')
        R.write("Results: {}".format(match)+'\n')
    R.close()

def splitParams(paramfile="5D_Malaria_2015_FCParams_MorseGraph565.txt",pieces=1):
    pfile=open(paramfile,'r')
    paramlist=list(pfile.readlines())
    pfile.close()
    paramsperfile=len(paramlist)/pieces
    if pieces*paramsperfile < paramlist:
        paramsperfile+=1
    for n in range(pieces):
        f=open(paramfile[:-4]+'_{:04d}'.format(n)+'.txt','w')
        for param in paramlist[n*paramsperfile:min([(n+1)*paramsperfile,len(paramlist)])]:
            f.write(param)
        f.close()

def mergeFiles(mergefile,splitfiles):
    m = open(mergefile,'w')
    for sfile in splitfiles:
        f=open(sfile,'r')
        for l in f.readlines():
            m.write(l)
        m.write("\n")
        f.close()
    m.close()

def setPattern_Malaria_20hr_2015_09_11():
    f=open('patterns.txt','w')
    for s1 in permutations(['x3 max','x5 max','x1 max', 'x4 max']):
        patternstr1=', '.join(s1) + ', x2 max, '
        for s2 in permutations(['x1 min', 'x3 min','x5  min', 'x2 min','x4 min']):
            patternstr2=patternstr1 + ', '.join(s2) + ', ' + s1[0] + '\n'
            f.write(patternstr2)
    f.close()

def setPattern_5D_Cycle():
    f=open('patterns.txt','w')
    f.write('X3 max, X4 max, X3 min, X4 min\n X3 max, X4 min, X3 min, X4 max')
    f.close()

def parallelrun_on_conley3(patternsetter=setPattern_Malaria_20hr_2015_09_11,morseset=0,networkfile="/home/bcummins/DSGRN/networks/5D_2015_09_11.txt",parambasedir="/share/data/bcummins/parameterfiles/",paramfile="5D_2015_09_11_FCParams_MorseGraph565.txt",resultsbasedir="/share/data/bcummins/parameterresults/",printtoscreen=0,printparam=0,findallmatches=0,numservers=0):
    # construct patterns
    patternsetter()
    # patternfunction()
    if numservers:
        job_server=pp.Server(ppservers=numservers)
    else:
        # tuple of all parallel python servers to connect with
        # ppservers = () 
        ppservers = ("*",) #CHANGED
        # Creates jobserver with automatically detected # of workers
        job_server = pp.Server(ppservers=ppservers)
    N = job_server.get_ncpus()
    print("Starting pp with " + str(N) + " workers.")
    # split the parameter file into N chunks
    splitParams(paramfile,N)
    # start the jobs
    jobs=[]
    allsubresultsfiles=[]
    paramname=paramfile.split('.')[0]
    parampath=parambasedir+paramname
    resultpath=resultsbasedir+paramname
    for i in range(N):
        subparamfile=parampath+'_{:04d}'.format(i)+'.txt'
        subresultsfile=resultpath+'_{:04d}'.format(i)+'.txt'
        allsubresultsfiles.append(subresultsfile)
        jobs.append(job_server.submit( patternSearch,(morseset,networkfile,subparamfile,subresultsfile,printtoscreen,printparam,findallmatches,i), (),modules = ("subprocess","patternmatch", "pp", "preprocess","fileparsers","walllabels","itertools","numpy","json"),globals=globals()))
    print "All jobs starting."
    for job in jobs:
        job()
    job_server.print_stats()
    mergeFiles(resultpath+'_merged_results.txt',allsubresultsfiles)

if __name__=='__main__':
    parambasedir="/share/data/bcummins/parameterfiles/"
    paramfile='5D_Cycle_FCParams_MorseGraph38479.txt'
    resultsbasedir='/share/data/bcummins/parameterresults/'
    patternsetter=setPattern_5D_Cycle
    networkfile="/home/bcummins/DSGRN/networks/5D_Cycle.txt"
    morseset=2
    parallelrun_on_conley3(patternsetter,morseset,networkfile,parambasedir,paramfile,resultsbasedir)

