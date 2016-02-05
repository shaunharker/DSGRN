import itertools, sys

def makeTotalOrdersFromLattice(partialorder,cyclic=1):
    # partialorder is list of lists of strings (extrema)
    # partialorder[i] < partialorder[i+1] for all i
    # only partial orders that are lattices are permitted 
    orders = [', '.join(list(s)) for s in itertools.permutations(partialorder[0])]
    for po in partialorder[1:]:
        orders=[t + ', ' + ', '.join(list(s)) for t in orders for s in itertools.permutations(po)]
    if cyclic:
        totalorders = []
        for t in orders:
            twords = t.split(',')
            totalorders.append(t + ', ' + twords[0])
    else:
        totalorders = orders
    return totalorders

def resultsFileParser(filename):
    uniquepatterns = []
    with open(filename,'r') as f: 
        for line in f:
            pattern = line.split(':')[-1][1:-1]
            if pattern not in uniquepatterns:
                uniquepatterns.append(pattern)
    return uniquepatterns

def searchFile(uniquepatterns,totalorders,lattice):
    count = 0
    for u in uniquepatterns:
        if u in totalorders:
            count+=1
    numuniq = len(uniquepatterns)
    print "Found {} patterns out of {} total patterns that are consistent with {}.".format(count,numuniq,lattice)
    return count, numuniq

def check5D_2016_01_28_originalpatterns(uniquepatterns):
    original_lattice = [ ['199 max', '177 max', '72 max','204 max'],
                ['77 max'],
                ['72 min', '199 min', '177  min', '77 min', '204 min'] ]    
    for k,o in enumerate(original_lattice):
        if len(o) > 1:
            ol1=original_lattice[:k]
            ol2=original_lattice[k+1:]
            for p in o:
                q = o[:]
                q.remove(p)
                new_lattice = ol1+[q]+[[p]]+ol2
                totalorders = makeTotalOrdersFromLattice(new_lattice,cyclic=1)
                searchFile(uniquepatterns,totalorders,new_lattice)
                new_lattice = ol1+[[p]]+[q]+ol2
                totalorders = makeTotalOrdersFromLattice(new_lattice,cyclic=1)
                searchFile(uniquepatterns,totalorders,new_lattice)


   


if __name__ == '__main__':
    # # filename='5D_2016_01_28_stableFCs_allresults.txt'
    # # uniquepatterns = resultsFileParser(filename)
    # # with open('uniquepatterns.txt','w') as uf:
    # #     for up in uniquepatterns:
    # #         uf.write(up+'\n')
    uniquepatterns=[]
    with open('uniquepatterns.txt','r') as uf:
        for line in uf:
            uniquepatterns.append(line[:-1])
    # check5D_2016_01_28_originalpatterns(uniquepatterns) 
    lattice = [ ['199 max'], ['177 max', '72 max'],['204 max'],
                ['77 max'], ['199 min'],['72 min', '177  min', '204 min'],['77 min']]    
    totalorders = makeTotalOrdersFromLattice(lattice,cyclic=1)
    searchFile(uniquepatterns,totalorders,lattice)