from subprocess import call
import numpy as np
import fileparsers as fp
import preprocess as pp

def testgraph(domains,edges):
    for k,(c,d) in enumerate(zip(domains,edges)):
        for i in range(n):
            diffs=[]
            for e in d:
                c2=domains[e]
                diffs.append(np.sign(np.mean(c[i]) - np.mean(c2[i])))
            if set([-1,1]).issubset(diffs):
                print k,c
                for e in d:
                    print e,domains[e]

def testwallgraph(domains,edges):
    for k,(c,d) in enumerate(zip(domains,edges)):
        for i in range(n):
            diffs=[]
            for e in d:
                c2=domains[e]
                diffs.append(np.sign(c[i] - c2[i]))
            if set([-1,1]).issubset(diffs):
                print k,c
                for e in d:
                    print e,domains[e]

def testdups(domains):
    for k,d in enumerate(domains):
        for j,o in enumerate(domains):
            if k != j and d==o:
                print k,j,d

def finddomain(cells,domgraph):
    cellind=-2
    for k,c in enumerate(cells):
        if c == [[1,2],[1,2],[1,2],[3,4],[0,1]]:
            cellind=k
            break
    if cellind==-2:
        print -2
        raise ValueError("Domain not in Morse set.")
    for e in domgraph:
        if cellind in e:
            if (cellind==e[0] and cells[e[1]]==[[1,2],[1,2],[1,2],[2,3],[0,1]]) or (cellind==e[1] and cells[e[0]]==[[1,2],[1,2],[1,2],[2,3],[0,1]]):
                edge=e
                print cellind, edge

specfile="networks/5D_Malaria_20hr.txt"
parameter=116014
morseset=0

call(["dsgrn network {} analyze morseset {} {} >dsgrn_output.json".format(specfile,morseset,parameter)],shell=True)
varnames,threshnames,domgraph,cells=fp.parseMorseSet('dsgrn_output.json')
n=len(cells[0])

finddomain(cells,domgraph)

# testdups(cells)

# testgraph(cells,domgraph)

# outedges,wallthresh,walldomains=pp.makeWallGraphFromDomainGraph(domgraph,cells) 

# testdups(walldomains)
# testwallgraph(walldomains,outedges)  


