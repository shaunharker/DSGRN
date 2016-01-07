def countDistinctParams(fname):
    with open(fname) as f:
        parameters=[]
        for line in f:
            parameters.append(line.split()[1][:-1])
    return len(set(parameters))

def countDistinctParamsInMorseGraph(fname,MGN):
    with open(fname) as f:
        parameters=[]
        for line in f:
            words = line.split()
            if int(words[3][:-1]) == MGN:
                parameters.append(words[1][:-1])
    return len(set(parameters))

if __name__=='__main__':
    fname = '/share/data/bcummins/parameterresults/5D_2016_01_05_A_stableFCs_allresults.txt'
    MGN = 1144
    print countDistinctParams(fname)
    print countDistinctParamsInMorseGraph(fname,MGN)