from itertools import permutations

def dataStruct43Genes():
    gene_name_master_list=['PF3D7_1115500','PF3D7_1006100','PF3D7_0504700','PF3D7_0604600','PF3D7_1408400','PF3D7_1027000','PF3D7_1103800','PF3D7_0614800','PF3D7_1435700','PF3D7_1405100','PF3D7_0403500','PF3D7_1301500','PF3D7_1428800','PF3D7_0506700','PF3D7_1350900','PF3D7_0925700','PF3D7_0518400','PF3D7_0529500','PF3D7_0809900','PF3D7_1008000','PF3D7_1009400','PF3D7_1225200','PF3D7_1337400','PF3D7_1437000','PF3D7_0313000','PF3D7_0627300','PF3D7_0629800','PF3D7_0704600','PF3D7_0729000','PF3D7_0812600','PF3D7_0818700','PF3D7_0915100','PF3D7_0919000','PF3D7_0926100','PF3D7_1119400','PF3D7_1138800','PF3D7_1225800','PF3D7_1227400','PF3D7_1233600','PF3D7_1237800','PF3D7_1302500','PF3D7_1406100','PF3D7_1412900']
    mins=[[9,12],[3,6],[12],[0,3,6],[0],[0,3,6,9,12],[0,3],[0,3],[0,3,6],[0],[0],[0],[15,18,21],[0],[12,15],[15,18],[12],[15,18,21],[12],[18],[15,18],[15,18],[12,15],[15,18],[15,18],[18],[12,15,18],[15,18,21,24],[12,15,18],[9,12],[12,15],[12,15],[12],[15,18],[15],[15,18,21],[12,15],[18],[12],[18,21],[12],[18,21,24],[12]]
    maxes=[[21],[18,21,24],[3],[21],[12,15,18],[18],[12,15,18,21],[15],[15,18],[12],[12],[12],[9],[9],[3],[3,6],[0],[0],[0],[0],[0],[0],[0],[0],[0],[0],[0],[0],[0],[0],[0],[0],[0],[0],[0],[0],[0],[0],[0],[0],[0],[0],[0]]
    return gene_name_master_list,[tuple(m) for m in mins],[tuple(m) for m in maxes]


def constructPartialOrder(datastruct=dataStruct43Genes):
    # construct set of unique times
    gene_name_master_list,mins,maxes=datastruct()
    unique_times=sorted(list(set(mins+maxes)))
    # construct transitive closure of the partial order
    outedges=[(unique_times.index(u),unique_times.index(v)) for u in unique_times for v in unique_times if max(u) < min(v)]
    # translate maxes and mins into set index
    setmins = [unique_times.index(m) for m in mins]
    setmaxes = [unique_times.index(M) for M in maxes]
    return gene_name_master_list,setmins,setmaxes, outedges

def getSubset(genes,setmins,setmaxes,gene_name_master_list):
    extrema=[]
    for g in genes:
        extrema.extend([(g+' min',setmins[gene_name_master_list.index(g)]),(g+' max',setmaxes[gene_name_master_list.index(g)])])
    return extrema

def extractSubGraph(extrema,outedges):
    times=[e[1] for e in extrema]
    return [oe for oe in outedges if oe[0] in times and oe[1] in times]

def identifyNoInComingEdges(extrema,outedges):
    base_set=[]
    for e in extrema:
        add=True
        for oe in outedges:
            if e[1]==oe[1]:
                add=False
                break
        if add:
            base_set.append(e[0])
    return base_set

def recurseOverGraph(extrema,outedges,pattern,filehandle):
    if len(extrema)==0:
        filehandle.write(pattern+'\n')
    else:
        # topological sort
        base_set=identifyNoInComingEdges(extrema,outedges)
        for b in base_set:
            fewer_extrema=[e for e in extrema if e[0] != b]
            fewer_outedges=extractSubGraph(fewer_extrema,outedges)
            recurseOverGraph(fewer_extrema,fewer_outedges,pattern+b+' ',filehandle)
            
def constructPatterns(genes,patternfile,datastruct=dataStruct43Genes):
    gene_name_master_list,setmins,setmaxes,outedges=constructPartialOrder(datastruct)
    extrema=getSubset(genes,setmins,setmaxes,gene_name_master_list)
    with open(patternfile,'w',0) as f:
        recurseOverGraph(extrema,outedges,'',f)
 


