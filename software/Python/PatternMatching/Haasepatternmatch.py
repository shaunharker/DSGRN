from itertools import permutations
import useDSGRN

def model5D_Model_B(morseset=0,specfile="networks/5D_Model_B.txt",paramfile="5D_Model_B_FCParams.txt",resultsfile='results_5D_B.txt',findallmatches=1):
    def setPattern():
        f=open('patterns.txt','w')
        for s1 in permutations(['Y1 max','Y2 max','Z max']):
            patternstr1='X max, ' + ', '.join(s1) + ', Y3 max, X min, '
            for s2 in permutations(['Y1 min','Y2 min','Z min']):
                patternstr2=patternstr1 + ', '.join(s2) + ', Y3 min, X max \n'
                f.write(patternstr2)
        f.close()
    setPattern()
    useDSGRN.patternSearch(morseset,specfile,paramfile,resultsfile,findallmatches=findallmatches)

def collectPatterns(fname='results_5D_B.txt'):
    f=open(fname,'r')
    patternsuccess=[]
    for l in f.readlines():
        if l.startswith('Pattern: '):
            patternsuccess.append(l[9:])
    for p in set(patternsuccess):
        print p
    f.close()

if __name__=='__main__':
    model5D_Model_B(paramfile="5D_Model_B_FCParams_first.txt",findallmatches=0)
    # collectPatterns('results_5D_B.txt')