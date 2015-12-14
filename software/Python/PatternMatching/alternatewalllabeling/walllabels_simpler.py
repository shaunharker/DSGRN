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

import itertools
from numpy import sign

def makeWallInfo(outedges,walldomains,varsaffectedatwall):
    # This function creates the dictionary used in the core recursive call for the pattern matching.
    inedges=[tuple([j for j,o in enumerate(outedges) if node in o]) for node in range(len(outedges))]  
    # precache wall differences
    outsigns=makeWallSigns(walldomains,outedges,True)
    insigns=makeWallSigns(walldomains,inedges,False)
    # make wall labels
    wallinfo=[]
    for (ins,outs,varatwall,oe) in zip(insigns,outsigns,varsaffectedatwall,outedges):
        walllabels=['']
        for varind in range(len(walldomains[0])): 
            isvaratwall=varind==varatwall
            chars=getTableValues(ins,outs,isvaratwall)
            walllabels=[l+c for l in walllabels for c in chars]
        wallinfo.append((oe,tuple(walllabels)))
    return wallinfo

def makeWallSigns(walldomains,edges,out):
    n=len(walldomains[0])
    edgesigns=[]
    for dom,ed in zip(walldomains,edges):
        alldifferences=[]
        for i in range(n):
            diffs=[]
            for e in ed:
                dom2=walldomains[e]
                diffs.append(sign(dom[i] - dom2[i]))
            if set([-1,1]).issubset(diffs):
                if out:
                    raise ValueError("The wall graph has spreading flow.")
                else: 
                    diffs=[0]
            alldifferences.append(1 if 1 in diffs else -1 if -1 in diffs else 0)
        edgesigns.append(alldifferences)
    return edgesigns

def getTableValues(insign,outsign,isvaratwall):
    if insign==-1 and outsign==-1:
        chars=['d']
    elif insign==1 and outsign==1:
        chars=['u']
    elif insign==-1 and outsign==1:
        chars=['m']
    elif insign == 1 and outsign==-1:
        chars=['M']
    elif insign==-1 and outsign==0:
        chars=['d','m']
    elif insign==1 and outsign==0:
        chars=['u','M']
    elif insign==0 and outsign==-1:
        chars=['d','M']
    elif insign ==0 and outsign==1:
        chars=['u','m']
    else:
        chars=['u','d','m','M']
    if not isvaratwall:
        C=set(chars)
        C.discard('M')
        C.discard('m')
        chars = list(C)
        if chars=='':
            raise ValueError("Wall labeling failed. A variable not affected at threshold is changing.")
    return chars


