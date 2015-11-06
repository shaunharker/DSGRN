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

from patternmatch import matchPattern
import preprocess as pp
import fileparsers as fp
import testcases as tc
from walllabels import makeWallInfo

def testme(showme=1):
    # find all matches
    test0(showme,findallmatches=1)
    test1(showme,findallmatches=1)
    test2(showme,findallmatches=1)
    test3(showme,findallmatches=1)
    test4(showme,findallmatches=1)
    test5(showme,findallmatches=1)
    test6(showme,findallmatches=1)
    # test7(showme,findallmatches=1)
    # test8(showme,findallmatches=1)
    # # find first match only
    test0(showme,findallmatches=0)
    test1(showme,findallmatches=0)
    test2(showme,findallmatches=0)
    test3(showme,findallmatches=0)
    test4(showme,findallmatches=0)
    test5(showme,findallmatches=0)
    test6(showme,findallmatches=0)
    # test7(showme,findallmatches=0)
    # test8(showme,findallmatches=0)

def test0(showme=1,findallmatches=1):
    wallinfo = makeWallInfo(*tc.test0())

    pattern=['md','um','Mu','dM','md']
    match = matchPattern(pattern,wallinfo,cyclic=1,findallmatches=findallmatches)
    if showme and findallmatches: print set(match)==set([(0, 1, 4, 6, 5, 2, 0), (3, 4, 6, 5, 3)])
    if showme and not findallmatches: print match[0] in [(0, 1, 4, 6, 5, 2, 0), (3, 4, 6, 5, 3)]

    pattern=['md','um','Mu','dM','md','um','Mu','dM','md'] #check double loop
    match = matchPattern(pattern,wallinfo,cyclic=1,findallmatches=findallmatches)
    if showme and findallmatches: print set(match)==set([(0, 1, 4, 6, 5, 2, 0, 1, 4, 6, 5, 2, 0), (3, 4, 6, 5, 3, 4, 6, 5, 3), (0, 1, 4, 6, 5, 3, 4, 6, 5, 2, 0), (3, 4, 6, 5, 2, 0, 1, 4, 6, 5, 3)])
    if showme and not findallmatches: print match[0] in [(0, 1, 4, 6, 5, 2, 0, 1, 4, 6, 5, 2, 0), (3, 4, 6, 5, 3, 4, 6, 5, 3), (0, 1, 4, 6, 5, 3, 4, 6, 5, 2, 0), (3, 4, 6, 5, 2, 0, 1, 4, 6, 5, 3)]

    pattern=['um','md'] #intermediate extrema
    match = matchPattern(pattern,wallinfo,cyclic=0,findallmatches=findallmatches)
    if showme: print 'None' in match

    pattern=['ud','um','Mu'] # acyclic 
    match = matchPattern(pattern,wallinfo,cyclic=0,findallmatches=findallmatches)
    if showme: print match==[(1,4,6)]

def test1(showme=1,findallmatches=1):
    wallinfo = makeWallInfo(*tc.test1())

    pattern=['md','um','Mu','dM','md']
    match = matchPattern(pattern,wallinfo,cyclic=1,findallmatches=findallmatches)
    if showme: print match==[(0, 1, 3, 2, 0)]

    pattern=['md','um','Mu','dM','Md'] # 'Md' DNE in graph
    match = matchPattern(pattern,wallinfo,cyclic=0,findallmatches=findallmatches)
    if showme: print 'None' in match 

    pattern=['md','Mu','dM','md'] # intermediate extrema
    match = matchPattern(pattern,wallinfo,cyclic=1,findallmatches=findallmatches)
    if showme: print 'None' in match

def test2(showme=1,findallmatches=1):
    wallinfo = makeWallInfo(*tc.test2())

    pattern=['dM','md','um','Mu','dM']
    match = matchPattern(pattern,wallinfo,cyclic=1,findallmatches=findallmatches)
    if showme and findallmatches: print set(match)==set([(2,0,1,3,2),(2,0,1,4,6,5,2)])
    if showme and not findallmatches: print match[0] in [(2,0,1,3,2),(2,0,1,4,6,5,2)]

    pattern=['Mu','dM','md','um','Mu']
    match = matchPattern(pattern,wallinfo,cyclic=1,findallmatches=findallmatches)
    if showme and findallmatches: print set(match)==set([(3,2,0,1,3),(6,5,2,0,1,4,6)])
    if showme and not findallmatches: print match[0] in [(3,2,0,1,3),(6,5,2,0,1,4,6)] 

    pattern=['um','Mu'] #acyclic
    match = matchPattern(pattern,wallinfo,cyclic=0,findallmatches=findallmatches)
    if showme and findallmatches: print set(match)==set([(1,4,6),(1,3)])
    if showme and not findallmatches: print match[0] in [(1,4,6),(1,3)]

def test3(showme=1,findallmatches=1):
    domaingraph,domaincells,morseset,vertexmap,outedges,walldomains,varsaffectedatwall,varnames,threshnames=tc.test3()
    extendedmorsegraph,extendedmorsecells=pp.makeExtendedMorseSetDomainGraph(vertexmap,morseset,domaingraph,domaincells)
    newoutedges,wallthresh,newwalldomains,booleanoutedges=pp.makeWallGraphFromDomainGraph(len(vertexmap),extendedmorsegraph, extendedmorsecells)
    newvarsaffectedatwall=pp.varsAtWalls(threshnames,newwalldomains,wallthresh,varnames)
    wallinfo = makeWallInfo(newoutedges,newwalldomains,newvarsaffectedatwall)
    wallinfo = pp.truncateExtendedWallGraph(booleanoutedges,newoutedges,wallinfo)
    patternnames,patternmaxmin,originalpatterns=fp.parsePatterns()
    patterns=pp.translatePatterns(varnames,patternnames,patternmaxmin,cyclic=1)
    match = matchPattern(patterns[0][0],wallinfo,cyclic=1,findallmatches=findallmatches)
    if showme: print match==[(5,0,1,2,3,4,5)]
    match = matchPattern(patterns[1][0],wallinfo,cyclic=1,findallmatches=findallmatches)
    if showme: print 'None' in match

def test4(showme=1,findallmatches=1):
    domaingraph,domaincells,morseset,vertexmap,outedges,walldomains,varsaffectedatwall,varnames,threshnames=tc.test4()
    extendedmorsegraph,extendedmorsecells=pp.makeExtendedMorseSetDomainGraph(vertexmap,morseset,domaingraph,domaincells)
    newoutedges,wallthresh,newwalldomains,booleanoutedges=pp.makeWallGraphFromDomainGraph(len(vertexmap),extendedmorsegraph, extendedmorsecells)
    newvarsaffectedatwall=pp.varsAtWalls(threshnames,newwalldomains,wallthresh,varnames)
    wallinfo = makeWallInfo(newoutedges,newwalldomains,newvarsaffectedatwall)
    wallinfo = pp.truncateExtendedWallGraph(booleanoutedges,newoutedges,wallinfo)
    patternnames,patternmaxmin,originalpatterns=fp.parsePatterns()
    patterns=pp.translatePatterns(varnames,patternnames,patternmaxmin,cyclic=1)
    match = matchPattern(patterns[0][0],wallinfo,cyclic=1,findallmatches=findallmatches)
    if showme and findallmatches: print set(match)==set([(4, 0, 2, 5, 4), (4, 0, 1, 3, 6, 5, 4)])
    if showme and not findallmatches: print match[0] in [(4, 0, 2, 5, 4), (4, 0, 1, 3, 6, 5, 4)]
    match = matchPattern(patterns[1][0],wallinfo,cyclic=1,findallmatches=findallmatches)
    if showme: print 'None' in match

def test5(showme=1,findallmatches=1):
    domaingraph,domaincells,morseset,vertexmap,outedges,walldomains,varsaffectedatwall,varnames,threshnames=tc.test5()
    extendedmorsegraph,extendedmorsecells=pp.makeExtendedMorseSetDomainGraph(vertexmap,morseset,domaingraph,domaincells)
    newoutedges,wallthresh,newwalldomains,booleanoutedges=pp.makeWallGraphFromDomainGraph(len(vertexmap),extendedmorsegraph, extendedmorsecells)
    newvarsaffectedatwall=pp.varsAtWalls(threshnames,newwalldomains,wallthresh,varnames)
    wallinfo = makeWallInfo(newoutedges,newwalldomains,newvarsaffectedatwall)
    wallinfo = pp.truncateExtendedWallGraph(booleanoutedges,newoutedges,wallinfo)
    patternnames,patternmaxmin,originalpatterns=fp.parsePatterns()
    patterns=pp.translatePatterns(varnames,patternnames,patternmaxmin,cyclic=1)
    match = matchPattern(patterns[0][0],wallinfo,cyclic=1,findallmatches=findallmatches)
    if showme and findallmatches: print set(match)==set([(6,0,3,4,5,10,9,8,6),(7,1,0,3,4,5,10,9,7),(7,2,4,5,10,9,7)])
    if showme and not findallmatches: print match[0] in [(6,0,3,4,5,10,9,8,6),(7,1,0,3,4,5,10,9,7),(7,2,4,5,10,9,7)]
    match = matchPattern(patterns[1][0],wallinfo,cyclic=1,findallmatches=findallmatches)
    if showme: print 'None' in match

def test6(showme=1,findallmatches=1):
    domaingraph,domaincells,morseset,vertexmap,outedges,walldomains,varsaffectedatwall,varnames,threshnames=tc.test6()
    extendedmorsegraph,extendedmorsecells=pp.makeExtendedMorseSetDomainGraph(vertexmap,morseset,domaingraph,domaincells)
    newoutedges,wallthresh,newwalldomains,booleanoutedges=pp.makeWallGraphFromDomainGraph(len(vertexmap),extendedmorsegraph, extendedmorsecells)
    newvarsaffectedatwall=pp.varsAtWalls(threshnames,newwalldomains,wallthresh,varnames)
    wallinfo = makeWallInfo(newoutedges,newwalldomains,newvarsaffectedatwall)
    wallinfo = pp.truncateExtendedWallGraph(booleanoutedges,newoutedges,wallinfo)
    patternnames,patternmaxmin,originalpatterns=fp.parsePatterns()
    patterns=pp.translatePatterns(varnames,patternnames,patternmaxmin,cyclic=1)
    solutions=[[(0, 5, 8, 9, 3, 1, 0), (0, 6, 11, 13, 16, 9, 3, 1, 0), (0, 6, 12, 14, 8, 9, 3, 1, 0), (0, 6, 12, 15, 16, 9, 3, 1, 0)],[(4,7,13,16,9,3,1,0,4)],[(10,7,13,16,10)],None,[(2,7,13,16,9,3,2)],None]
    patterns=[p for pat in patterns for p in pat]
    for p,s in zip(patterns,solutions):
        match = matchPattern(p,wallinfo,cyclic=1,findallmatches=findallmatches)
        if s:
            if showme and findallmatches: print set(match)==set(s)
            if showme and not findallmatches: print match[0] in s
        else:
            if showme: print 'None' in match and 'Pattern' in match

def test7(showme=1,findallmatches=1):
    tc.test7()
    patterns,originalpatterns,wallinfo = pp.preprocess(cyclic=1)
    solutions=[None,None,[(1,2,3,4,5,0,1)],[(4,5,0,1,2,3,4)]]
    patterns=[p for pat in patterns for p in pat]
    for p,s in zip(patterns,solutions):
        match = matchPattern(p,wallinfo,cyclic=1,findallmatches=findallmatches)
        if s:
            if showme: print match==s
        else:
            if showme: print 'None' in match and 'Pattern' in match

def test8(showme=1,findallmatches=1):
    tc.test8()
    patterns,originalpatterns,wallinfo = pp.preprocess(cyclic=1)
    solutions=[[(1,2,6,0,1),(1, 3, 4, 5, 6, 0, 1)]]*4+[None]*4
    patterns=[p for pat in patterns for p in pat]
    for p,s in zip(patterns,solutions):
        match = matchPattern(p,wallinfo,cyclic=1,findallmatches=findallmatches)
        if s:
            if showme and findallmatches: print set(match)==set(s)
            if showme and not findallmatches: print match[0] in s
        else:
            if showme: print 'None' in match and 'Pattern' in match


if __name__=='__main__':
    testme()
