from patternsnatcher import dataStruct43Genes
from math import sqrt

def parseFile(bound=0,fname='/Users/bcummins/ProjectData/malaria/wrair2015_pfalcip_462TF_forLEM/wrair2015_pfalc_462tf_lem.allscores.tsv'):
    f=open(fname,'r')
    source=[]
    regulation=[]
    target=[]
    lem_score=[]
    f.readline()
    for l in f.readlines():
        wordlist=l.split()
        if float(wordlist[3])>bound:
            source.append(wordlist[0])
            lem_score.append(float(wordlist[3]))
            two_words=wordlist[2].split('(')
            regulation.append(two_words[0])
            target.append(two_words[1][:-1])
        else:
            break
    f.close()
    return source,target,regulation,lem_score

def max_scores(in_score,out_score):
    return max([in_score,out_score])

def min_scores(in_score,out_score):
    return min([in_score,out_score])

def add_scores(in_score,out_score):
    return in_score+out_score

def average_scores(in_score,out_score):
    return (in_score+out_score)/2.0

def geometric_average_scores(in_score,out_score):
    return sqrt(in_score*out_score)

def harmonic_average_scores(in_score,out_score):
    return 2.0 / ( (in_score)**(-1) + (out_score)**(-1) )

def findNewNodeAndConnections(network_names,network_outedges,network_regulation,ranking_function=add_scores):
    # get all gene names
    gene_name_master_list,_,_=dataStruct43Genes()
    # get given network edges
    network_sources=[]
    network_targets=[]
    for k,oe in enumerate(network_outedges):
        for o in oe:
            network_sources.append(network_names[k])
            network_targets.append(network_names[o])
    # get all possible (nonzero) edges
    source,target,regulation,lem_score=parseFile()
    # pick the best new node
    best_score=[]
    best_edges=[]
    for g in gene_name_master_list:
        inedge=()
        in_score=0
        outedge=()
        out_score=0
        for n,s,t,r in enumerate(zip(source,target,regulation)):
            if s == g and t in network_names:
                out_score=43-n
                outedge=(s,t,r)
            elif t == g and s in network_names:  
                in_score=43-n
                inedge=(s,t,r)
        best_score.append(ranking_function(in_score,out_score))
        best_edges.append((outedge,inedge))
    add_node=best_edges[best_score.index(max(best_score))]
    return add_node




    


if __name__=='__main__':
    source,target,type_reg,lem_score=parseFile()
    print target[0]