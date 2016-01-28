import sys

def countDistinctParams(fname):
    with open(fname) as f:
        parameters=[]
        for line in f:
            if line != '\n':
                parameters.append(line.split()[1][:-1])
    return len(set(parameters))

def countDistinctParamsInMorseGraph(fname,MGN):
    with open(fname) as f:
        parameters=[]
        for line in f:
            if line != '\n':
                words = line.split()
                if int(words[4][:-1]) == MGN:
                    parameters.append(words[1][:-1])
    return len(set(parameters))

if __name__=='__main__':
    #fname = '/share/data/bcummins/parameterresults/5D_2016_01_16_A_MG4578_allresults.txt'
    # MGN = 4578
    print countDistinctParams(sys.argv[1])
    # print countDistinctParamsInMorseGraph(fname,MGN)
