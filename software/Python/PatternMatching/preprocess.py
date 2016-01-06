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

import walllabels as wl
# import fileparsers as fp
import itertools, sys
import json

def preprocess(morseset_jsonstr,domgraph_jsonstr,domaincells_jsonstr,patternstr,cyclic=1):
    # read json strings
    varnames,threshnames,morsedomgraph,morsecells,vertexmap,domaingraph,domaincells=extractFromJSON(morseset_jsonstr,domgraph_jsonstr,domaincells_jsonstr)
    # read pattern string
    patternnames,patternmaxmin,originalpatterns=parsePatternString(patternstr)
    # put max/min patterns in terms of the alphabet u,m,M,d
    patterns=translatePatterns(varnames,patternnames,patternmaxmin,cyclic=cyclic)
    # translate domain graph into extended wall graph
    extendedmorsegraph,extendedmorsecells=reindexDomainGraph(vertexmap,morsecells,domaingraph,domaincells)
    outedges,wallthresh,walldomains,booleanoutedges=makeWallGraphFromDomainGraph(len(vertexmap),extendedmorsegraph, extendedmorsecells)
    # record which variable is affected at each wall
    varsaffectedatwall=varsAtWalls(threshnames,walldomains,wallthresh,varnames)
    # make wall labels with extended wall graph
    wallinfo = wl.makeWallInfo(outedges,walldomains,varsaffectedatwall)
    # REQUIRED: truncate back to Morse wall graph so that the pattern search is only over the morse set
    # However, reindexing may not be necessary
    wallinfo = truncateExtendedWallGraph(booleanoutedges,outedges,wallinfo)
    return patterns, originalpatterns, wallinfo

def extractFromJSON(morseset_jsonstr,domgraph_jsonstr,domaincells_jsonstr):
    # read json strings
    parsed=json.loads(morseset_jsonstr)
    varnames = [ x[0] for x in parsed["network"] ]
    threshnames = [ [parsed["network"][i][2][j] for j in parsed["parameter"][i][2]] for i in range(len(parsed["network"])) ]
    morsedomgraph=parsed["graph"]
    morsecells=parsed["cells"]
    vertexmap=parsed["vertices"]
    domaingraph=json.loads(domgraph_jsonstr)
    domaincells=json.loads(domaincells_jsonstr)['cells']
    return varnames,threshnames,morsedomgraph,morsecells,vertexmap,domaingraph,domaincells

def parsePatternString(patternstr):
    maxmin=[]
    patternnames=[]
    originalpatterns=[]
    patternlist = patternstr.split('\n ')
    for pat in patternlist:
        originalpatterns.append(pat)
        p=pat.replace(',',' ').split()
        patternnames.append(p[::2])
        maxmin.append(p[1::2])
    return patternnames, maxmin, originalpatterns

def translatePatterns(varnames,patternnames,patternmaxmin,cyclic=0):
    numvars=len(varnames)
    patterninds=[[varnames.index(q) for q in p] for p in patternnames]
    patterns=[]
    # loop over each provided pattern
    for inds,extrema in zip(patterninds,patternmaxmin):
        length_pattern = len(inds)
        # record locations of extrema for each variable
        variable_extrema=[['0' if k != i else 'M' if e=='max' else 'm' for i,e in zip(inds,extrema)] for k in range(numvars)]
        # make sure the pattern makes physical sense (no two identical extrema in a row)
        if isGoodPattern(variable_extrema,inds,extrema):
            # build a set of template patterns if there are missing variables in the pattern (these could either be 'u' or 'd')
            templates=makeTemplates(numvars,inds,variable_extrema,length_pattern)
            # for each template, fill in the remaining blanks based on the location of the extrema, and knit the variable sequences into a pattern
            pats=[]
            for template in templates:
                pats.append(makePattern(template,length_pattern,cyclic))
            patterns.append(pats)                
    return patterns

def isGoodPattern(variable_extrema,inds,extrema):
    for varext in variable_extrema:
        filt_varext = filter(None,[v if v in ['m','M'] else None for v in varext])
        if set(filt_varext[::2])==set(['m','M']) or set(filt_varext[1::2])==set(['m','M']):
            print "Pattern {} is not consistent. Every variable must alternate maxima and minima. Removing from search.".format(zip(inds,extrema))
            return False
    return True

def makeTemplates(numvars,inds,variable_extrema,length_pattern):
    missingvars=sorted(list(set(range(numvars)).difference(set(inds))))
    if missingvars:
        templates=[]
        for c in itertools.combinations_with_replacement(['u','d'],len(missingvars)):
            new_template=[ [c[missingvars.index(k)]]*length_pattern if k in missingvars else variable_extrema[k] for k in range(numvars)]
            templates.append(new_template)
    else:
        templates=[variable_extrema]
    return templates

def makePattern(template,length_pattern,cyclic):
    # use locations of extrema to fill in blanks; for example, 'M00m0' --> 'Mddmu', by inspection of extrema, where 'Mddmu' = max down down min up
    for i,t in enumerate(template):
        for j in range(length_pattern):
            if t[j]=='0':
                J,K=j,j
                while t[K]=='0' and K>0:
                    K-=1
                while t[J]=='0' and J<length_pattern-1:
                    J+=1
                t[j] = 'd' if t[K] in ['M','d'] or t[J] in ['m','d'] else 'u'
        template[i]=t
    # knit the individual variable sequences into the words of the pattern
    # for example, ['uuMd','dmuu','Mddm'] --> ['udM','umd','Mud','dum']
    pattern = [''.join([t[k] for t in template]) for k in range(length_pattern)]
    # if a cyclic pattern is desired, make sure first and last elements are the same
    if cyclic and pattern[0] != pattern[-1]: 
        pattern.append(pattern[0])
    return pattern
 
def varsAtWalls(threshnames,walldomains,wallthresh,varnames):
    varsaffectedatthresh=[]
    for t in threshnames:
        varsaffectedatthresh.append(tuple([varnames.index(u) for u in t]))
    varsaffectedatwall=[-1]*len(walldomains)
    for k,(j,w) in zip(wallthresh,enumerate(walldomains)):
        if k != None and abs(w[k]-int(w[k]))<0.25: # is abs val check necessary?
            varsaffectedatwall[j]=varsaffectedatthresh[k][int(w[k]-1)]
        elif k is None:
            varsaffectedatwall[j]=None
    if any([v<0 for v in varsaffectedatwall]):
        raise ValueError('Affected variables are undefined at some walls.')
    return varsaffectedatwall

def makeWallGraphFromDomainGraph(morsegraphlen,domgraph,cells):
    # number of domains in morse set
    morseinds=range(morsegraphlen)
    # make domain edges (walls) except for self-loops
    domedges=[(k,d) for k,e in enumerate(domgraph) for d in e if k != d]
    # construct wall domains and record each variable at threshold
    wallthresh=[]
    walldomains=[]
    for de in domedges:
        c0=cells[de[0]]
        c1=cells[de[1]]
        n=len(c0)
        location=[True if c0[k]!=c1[k] else False for k in range(n)]
        if sum(location) > 1: 
            raise ValueError("The domain graph has an edge between nonadjacent domains. Aborting.")
        elif sum(location)==0: 
            raise ValueError("Self-loop in domain graph. Aborting.")
        else:
            wallthresh.append(location.index(True))
            walldomains.append(tuple([sum(c0[k]+c1[k])/4.0 for k in range(n)])) # the addition operator is list concatenation: sum([0,1]+[1,2])/4.0 = 4/4 = 1
    # convert domain edges into wall edges
    booleanwallgraph=[]
    wallgraph=[]
    for k,edge1 in enumerate(domedges):
        for j,edge2 in enumerate(domedges):
            if edge1[1]==edge2[0]:
                wallgraph.append((k,j))
                if edge1[0] in morseinds and edge1[1] in morseinds and edge2[1] in morseinds:
                    booleanwallgraph.append(True)
                else:
                    booleanwallgraph.append(False)  
    # record the wall graph                     
    outedges=[[] for _ in range(len(domedges))]
    booleanoutedges=[[] for _ in range(len(domedges))]
    for e,b in zip(wallgraph,booleanwallgraph):
        outedges[e[0]].append(e[1])
        booleanoutedges[e[0]].append(b)
    outedges=[tuple(o) for o in outedges]
    booleanoutedges=[tuple(o) for o in booleanoutedges]
    return outedges,wallthresh,walldomains,booleanoutedges

def reindexDomainGraph(vertexmap,morsecells,domaingraph,domaincells):    
    # add all remaining domains with new indices
    newdomains = [k for k in range(len(domaingraph)) if k not in vertexmap]
    newvertexmap = vertexmap + newdomains
    # extend the morse domain graph and cells
    extendedmorsecells=morsecells+[domaincells[v] for v in newdomains]
    extendedmorsegraph=[[newvertexmap.index(e) for e in domaingraph[v]] for v in newvertexmap]
    return extendedmorsegraph, extendedmorsecells

def truncateExtendedWallGraph(booleanoutedges,outedges,wallinfo):
    # take the extra walls out of wallinfo
    flatoutedges=[(k,o) for k,oe in enumerate(outedges) for o in oe]
    flatbooledge=[b for be in booleanoutedges for b in be]
    wallvertexmap=[]
    for k,(oe,boe) in enumerate(zip(outedges,booleanoutedges)):
        if any(boe):
            wallvertexmap.append(k)
        for o,b in zip(oe,boe):
            if b:
                labels=wallinfo[(k,o)]
                newlabels=[]
                for l in labels:
                    if flatbooledge[flatoutedges.index((o,l[0]))]:
                        newlabels.append(l)
                wallinfo[(k,o)] = newlabels
            else:
                wallinfo.pop((k,o), None)    
    # translate to new wall indices
    newwallinfo={}
    for key,labels in wallinfo.iteritems():
        newwallinfo[(wallvertexmap.index(key[0]), wallvertexmap.index(key[1]))] = [tuple([wallvertexmap.index(next),lab]) for next,lab in labels]
    return newwallinfo


if __name__=='__main__':
    # varnames,threshnames,morsedomgraph,morsecells,vertexmap=fp.parseMorseSet()
    # domaingraph=fp.parseDomainGraph()
    # domaincells=fp.parseDomainCells()
    # print makeExtendedMorseSetDomainGraph(vertexmap,morsecells,domaingraph,domaincells)
    patternstr = 'Z min, X min, Y min, Z max, X max, Y max\n X max, Y max, Z max, X min, Y min, Z min\n X min, Y max, Z min, X max, Y min, Z max\n X max, Y min, Z max, X min, Y max, Z min'
    patternnames,patternmaxmin,originalpatterns=parsePatternString(patternstr)
    print originalpatterns
    print patternnames
    print patternmaxmin