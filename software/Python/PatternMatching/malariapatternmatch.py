import findmorsegraph as fmg
import patternmatch as pm
from itertools import permutations
from subprocess import call
import sys


def getAllParams(fname="networks/6D_Malaria.txt",smallestparam=0,largestparam=46656,getMorseSet=fmg.is_FC):
    params=fmg.scan(fname,smallestparam,largestparam,getMorseSet,firstonly=False)
    f=open('storeparams.txt','w')
    f.write(str(params))
    f.close()
    return params

def parseParams(fname="storeparams.txt"):
    f=open(fname,'r')
    params=eval(f.readline())
    f.close()
    return params

def setPattern():
    f=open('patterns.txt','w')
    for s1 in permutations(['x6 min','x2 min','x3 max','x5 max','x4 max', 'x1 min']):
        patternstr1=', '.join(s1) + ', x2 max, x6 max, '
        for s2 in permutations(['x1 max', 'x3 min','x5 min','x4 min']):
            patternstr2=patternstr1 + ', '.join(s2) + ', ' + s1[0] + '\n'
            f.write(patternstr2)
    f.close()

def patternSearch(fname="networks/6D_Malaria.txt",smallestparam=0,largestparam=46656,getMorseSet=fmg.all_FC,paramsstored=1,resultsfile='malariaresults.txt'):
    if paramsstored == 1:
        params = parseParams()
    else:
        params = getAllParams(fname,smallestparam,largestparam,getMorseSet)
    N=len(params)
    f=open(resultsfile,'w')
    for k,(p,m) in enumerate(params):
        print "Parameter {} of {}".format(k+1,N)
        if type(m) is tuple:
            for n in m:
                call(["dsgrn network {} analyze morseset {} {} >dsgrn_output.json".format(fname,n,p)],shell=True)
                patterns,matches=pm.callPatternMatch(writetofile=0,returnmatches=1)
                for q,c in zip(patterns,matches):
                    f.write("Parameter: {}, Morseset: {}".format(p,n)+'\n')
                    f.write("Pattern: {}".format(q)+'\n')
                    f.write("Results: {}".format(c)+'\n')
        else:
            call(["dsgrn network {} analyze morseset {} {} >dsgrn_output.json".format(fname,m,p)],shell=True)
            patterns,matches=pm.callPatternMatch(writetofile=0,returnmatches=1)
            for q,c in zip(patterns,matches):
                f.write("Parameter: {}, Morseset: {}".format(p,m)+'\n')
                f.write("Pattern: {}".format(q)+'\n')
                f.write("Results: {}".format(c)+'\n')
    f.close()


if __name__=='__main__':
    setPattern()
    patternSearch(getMorseSet=fmg.all_FC,paramsstored=0)



