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
    # make every triple and the list of associated wall labels; store in dict indexed by (inedge,wall)
    wallinfo={}
    for currentwall,(ie,oe) in enumerate(zip(inedges,outedges)):
       for previouswall,nextwall in itertools.product(ie,oe):
            # construct the wall label for every permissible triple
            triple=(previouswall,currentwall,nextwall)
            varatwall=varsaffectedatwall[currentwall]
            try:
                labels=pathDependentLabelConstruction(triple,insigns,outsigns,walldomains,varatwall)
            except ValueError:
                # print 'triple, domain, var affected:'
                # for i in triple:
                #     print i,walldomains[i],varsaffectedatwall[i]
                # print 'previous out:'
                # for i in outedges[triple[0]]:
                #     print i,walldomains[i],varsaffectedatwall[i]
                # print 'current in:'
                # for i in inedges[triple[1]]:
                #     print i,walldomains[i],varsaffectedatwall[i]
                # print 'current out:'
                # for i in outedges[triple[1]]:
                #     print i,walldomains[i],varsaffectedatwall[i]
                # print 'next in:'
                # for i in inedges[triple[2]]:
                #     print i,walldomains[i],varsaffectedatwall[i]
                # print 'walls in second domain:'
                # for edges,j in zip([inedges[j] for j in outedges[triple[1]]],outedges[triple[1]]):
                #     for i in edges:
                #         print i,walldomains[i],varsaffectedatwall[i],str(i)+'->'+str(j)
                raise
            # Put the result in the dictionary.
            key=(previouswall,currentwall)
            value=(nextwall,labels)
            # If the key already exists, append to its list. Otherwise start the list.
            if key in wallinfo:
                wallinfo[key].append(value)
            else:
                wallinfo[key]=[value]
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

def pathDependentLabelConstruction(triple,insigns,outsigns,walldomains,varatwall):
    # make a label for the given triple
    if triple[1]==triple[2]: # can't handle steady states
        raise ValueError('Aborting: Wall has a self-loop.')
    walllabels=['']
    # for every variable find allowable letters for triple
    for varind in range(len(walldomains[0])): 
        isvaratwall=varind==varatwall
        varvalues=tuple([walldomains[k][varind] for k in triple])
        # try simple algorithm first
        chars=getChars(isvaratwall,varvalues) 
        if chars is None:
            # now try complex algorithm
            if isvaratwall:
                # use extra information to get the characters when extrema are allowed
                chars=getCharsExtrema(varind,triple,insigns,outsigns)
            else:
                # use extra information to get the characters when extrema are not allowed
                try:
                    chars=getCharsNoExtrema(varind,triple,insigns,outsigns)
                except ValueError:
                    # print varind, varatwall
                    # print triple[0],walldomains[triple[0]]
                    # print triple[1],walldomains[triple[1]]
                    # print triple[2],walldomains[triple[2]]
                    # print outsigns[triple[0]]
                    # print insigns[triple[1]]
                    # print outsigns[triple[1]]
                    # print insigns[triple[2]]
                    raise
        # make every combination of characters in the growing labels
        walllabels=[l+c for l in walllabels for c in chars]

    return tuple(walllabels)

def getChars(isvaratwall,(prev,curr,next)):
    chars=None
    if prev<curr<next:
        chars = ['u']
    elif prev>curr>next:
        chars = ['d']
    elif isvaratwall: # extrema allowed
        if prev<curr>next:
            chars=['M'] 
        elif prev>curr<next:
            chars=['m']
    elif not isvaratwall: # extrema not allowed
        if prev<curr>next or prev>curr<next:
            raise ValueError("Wall labeling failed. A variable not affected at threshold is changing.")
        elif prev<curr==next or prev==curr<next:
            chars = ['u']
        elif prev>curr==next or prev==curr>next:
            chars = ['d']
    return chars

def getCharsExtrema(varind,(prev,curr,next),insigns,outsigns):
    if ( outsigns[prev][varind]==-1 or insigns[curr][varind]==1 ) and ( outsigns[curr][varind]==1 or insigns[next][varind]==-1 ):
        chars=['M']
    elif ( outsigns[prev][varind]==1 or insigns[curr][varind]==-1 ) and ( outsigns[curr][varind]==-1 or insigns[next][varind]==1 ):
        chars=['m']
    elif ( outsigns[prev][varind]==-1 or insigns[curr][varind]==1 ) and ( outsigns[curr][varind]==-1 or insigns[next][varind]==1 ):
        chars=['u']
    elif ( outsigns[prev][varind]==1 or insigns[curr][varind]==-1 ) and ( outsigns[curr][varind]==1 or insigns[next][varind]==-1 ):
        chars=['d']
    elif ( outsigns[prev][varind]==-1 or insigns[curr][varind]==1 ):
        chars=['u','M']
    elif ( outsigns[prev][varind]==1 or insigns[curr][varind]==-1 ):
        chars=['d','m']
    elif ( outsigns[curr][varind]==1 or insigns[next][varind]==-1 ):
        chars=['d','M']
    elif ( outsigns[curr][varind]==-1 or insigns[next][varind]==1 ):
        chars=['u','m']
    else:
        chars=['u','d','m','M']
    return chars

def getCharsNoExtrema(varind,(prev,curr,next),insigns,outsigns):
    if ( ( outsigns[prev][varind]==-1 or insigns[curr][varind]==1 ) and ( outsigns[curr][varind]==1 or insigns[next][varind]==-1 ) ) or ( ( outsigns[prev][varind]==1 or insigns[curr][varind]==-1 ) and ( outsigns[curr][varind]==-1 or insigns[next][varind]==1 )  ):
        raise ValueError("Wall labeling failed. A variable not affected at threshold is changing.")
    elif outsigns[prev][varind]==-1 or insigns[curr][varind]==1 or outsigns[curr][varind]==-1 or insigns[next][varind]==1:
        chars=['u']
    elif outsigns[prev][varind]==1 or insigns[curr][varind]==-1 or outsigns[curr][varind]==1 or insigns[next][varind]==-1:
        chars=['d']
    else:
        chars=['d','u']
    return chars




