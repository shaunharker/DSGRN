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
import preprocess as pp
import testcases as tc

def testme():
    test0()
    test1()
    test2()
    test3()
    test4()
    test5()
    test6()

def test0():
    outedges,walldomains,varsaffectedatwall=tc.test0()
    wallinfo = wl.makeWallInfo(outedges,walldomains,varsaffectedatwall)
    print wallinfo[(3,4)]==[(6,('um',))]
    print wallinfo[(1,4)]==[(6,('um',))]
    print wallinfo[(2,0)]==[(1,('md',))]
    print set(wallinfo[(6,5)])==set([(2,('dM',)),(3,('dM',))])
    # print wl.infoFromWalls(0,walldomains[4][0],[1,3],walldomains)==(True,False)

def test1():
    outedges,walldomains,varsaffectedatwall=tc.test1()
    wallinfo = wl.makeWallInfo(outedges,walldomains,varsaffectedatwall)
    print wallinfo[(0,1)]==[(3,('um',))]
    print wallinfo[(1,3)]==[(2,('Mu',))]
    print wallinfo[(3,2)]==[(0,('dM',))]
    print wallinfo[(2,0)]==[(1,('md',))]
    # print wl.infoFromWalls(1,walldomains[2][1],[3],walldomains)==(True,False)

def test2():
    outedges,walldomains,varsaffectedatwall=tc.test2()
    wallinfo = wl.makeWallInfo(outedges,walldomains,varsaffectedatwall)
    print set(wallinfo[(0,1)])==set([(3,('um',)),(4,('um',))])
    print wallinfo[(3,2)]==[(0,('dM',))]
    print wallinfo[(5,2)]==[(0,('dM',))]
    print wallinfo[(1,4)]==[(6,('uu',))]
    # print wl.infoFromWalls(1,walldomains[1][1],[3,4],walldomains)==(False,True)

def test3():
    domaingraph,domaincells,morseset,vertexmap,outedges,walldomains,varsaffectedatwall,varnames,threshnames=tc.test3()
    extendedmorsegraph,extendedmorsecells=pp.makeExtendedMorseSetDomainGraph(vertexmap,morseset,domaingraph,domaincells)
    newoutedges,wallthresh,newwalldomains,booleanoutedges=pp.makeWallGraphFromDomainGraph(len(vertexmap),extendedmorsegraph, extendedmorsecells)
    newvarsaffectedatwall=pp.varsAtWalls(threshnames,newwalldomains,wallthresh,varnames)
    wallinfo = wl.makeWallInfo(newoutedges,newwalldomains,newvarsaffectedatwall)
    wallinfo = pp.truncateExtendedWallGraph(booleanoutedges,newoutedges,wallinfo)
    print set(wallinfo.keys())==set([(0,1),(1,2),(2,3),(3,4),(4,5),(5,0)])
    print wallinfo[(0,1)]==[(2,('dmu',))]   
    print wallinfo[(1,2)]==[(3,('duM',))]   
    print wallinfo[(2,3)]==[(4,('mud',))]   
    print wallinfo[(3,4)]==[(5,('uMd',))]   
    print wallinfo[(4,5)]==[(0,('udm',))]   
    print wallinfo[(5,0)]==[(1,('Mdu',))]   

def test4():
    domaingraph,domaincells,morseset,vertexmap,outedges,walldomains,varsaffectedatwall,varnames,threshnames=tc.test4()
    extendedmorsegraph,extendedmorsecells=pp.makeExtendedMorseSetDomainGraph(vertexmap,morseset,domaingraph,domaincells)
    newoutedges,wallthresh,newwalldomains,booleanoutedges=pp.makeWallGraphFromDomainGraph(len(vertexmap),extendedmorsegraph, extendedmorsecells)
    newvarsaffectedatwall=pp.varsAtWalls(threshnames,newwalldomains,wallthresh,varnames)
    wallinfo = wl.makeWallInfo(newoutedges,newwalldomains,newvarsaffectedatwall)
    wallinfo = pp.truncateExtendedWallGraph(booleanoutedges,newoutedges,wallinfo)
    print set(wallinfo.keys())==set([(0,1),(0,2),(1,3),(2,5),(3,6),(4,0),(5,4),(6,5)])
    print wallinfo[(0,1)]==[(3,('ud',))]
    print wallinfo[(0,2)]==[(5,('um',))]
    print wallinfo[(1,3)]==[(6,('um',))]
    print wallinfo[(2,5)]==[(4,('Mu',))]
    print wallinfo[(3,6)]==[(5,('uu',))]
    print set(wallinfo[(4,0)])==set([(1,('md',)),(2,('md',))])
    print wallinfo[(5,4)]==[(0,('dM',))]
    print wallinfo[(6,5)]==[(4,('Mu',))]

def test5():
    domaingraph,domaincells,morseset,vertexmap,outedges,walldomains,varsaffectedatwall,varnames,threshnames=tc.test5()
    extendedmorsegraph,extendedmorsecells=pp.makeExtendedMorseSetDomainGraph(vertexmap,morseset,domaingraph,domaincells)
    newoutedges,wallthresh,newwalldomains,booleanoutedges=pp.makeWallGraphFromDomainGraph(len(vertexmap),extendedmorsegraph, extendedmorsecells)
    print newwalldomains==walldomains+[(0.5,2,0.5),(1,2.5,0.5),(0.5,2.5,1),(1.5,0.5,1),(1,0.5,1.5),(1.5,1,1.5),(1.5,1.5,1),(1,1.5,1.5),(1.5,2,1.5)]
    print newoutedges==outedges+[(1, 2), (5,), (9,), (3,), (6,), (17, 18, 19), (4,), (7, 8), (10,)]
    newvarsaffectedatwall=pp.varsAtWalls(threshnames,newwalldomains,wallthresh,varnames)
    print newvarsaffectedatwall[:11]==varsaffectedatwall
    wallinfo = wl.makeWallInfo(newoutedges,newwalldomains,newvarsaffectedatwall)
    wallinfo = pp.truncateExtendedWallGraph(booleanoutedges,newoutedges,wallinfo)
    print set(wallinfo.keys())==set([(0,3),(1,0),(2,4),(3,4),(4,5),(5,10),(6,0),(7,1),(7,2),(8,6),(9,7),(9,8),(10,9)])
    print wallinfo[(0,3)]==[(4,('uud',))]
    print wallinfo[(1,0)]==[(3,('umd',))]
    print wallinfo[(2,4)]==[(5,('uum',))]
    print wallinfo[(3,4)]==[(5,('uum',))]
    print wallinfo[(4,5)]==[(10,('Muu',))]
    print wallinfo[(5,10)]==[(9,('dMu',))]
    print wallinfo[(6,0)]==[(3,('umd',))]
    print wallinfo[(7,1)]==[(0,('udd',))]
    print wallinfo[(7,2)]==[(4,('umd',))]
    print wallinfo[(8,6)]==[(0,('mdd',))]
    print set(wallinfo[(9,7)])==set([(1,('mdd',)),(2,('mdd',))])
    print set(wallinfo[(10,9)])==set([(7,('ddM',)),(8,('ddM',))])

def test6():
    domaingraph,domaincells,morseset,vertexmap,outedges,walldomains,varsaffectedatwall,varnames,threshnames=tc.test6()
    extendedmorsegraph,extendedmorsecells=pp.makeExtendedMorseSetDomainGraph(vertexmap,morseset,domaingraph,domaincells)
    print extendedmorsegraph==[[12,3],[0,4],[11,1,12],[4,5,7],[8],[12,6],[2,4],[8,9],[10],[5,10],[6],[],[]]
    newoutedges,wallthresh,newwalldomains,booleanoutedges=pp.makeWallGraphFromDomainGraph(len(vertexmap),extendedmorsegraph, extendedmorsecells)
    print newwalldomains==[
    (1.5,0.5,1),
    (2,0.5,0.5),
    (1.5,1,0.5),
    (2,1.5,0.5),
    (1,1.5,1.5),
    (1.5,1.5,1),
    (1.5,1,1.5),
    (2.5,1,0.5),
    (2.5,0.5,1),
    (3,0.5,0.5),
    (3,1.5,0.5),
    (2,0.5,1.5),
    (2.5,1,1.5),
    (2,1.5,1.5),
    (2.5,1.5,1),
    (3.5,1,0.5),
    (3.5,0.5,1),
    (3.5,1.5,1),
    (3,0.5,1.5),
    (3.5,1,1.5),
    (3,1.5,1.5)
    ]
    print newoutedges==[(),(7,8,9),(0,1),(10,),(),(2,3),(),(10,),(11,12),(15,16),(17,),(),(13,14),(4,5,6),(10,),(17,),(18,19),(20,),(11,12),(20,),(13,14)]
    newvarsaffectedatwall=pp.varsAtWalls(threshnames,newwalldomains,wallthresh,varnames)
    print newvarsaffectedatwall==[0,1,2,1,0,0,2,2,0,2,2,1,2,1,0,2,0,0,2,2,2]
    wallinfo = wl.makeWallInfo(newoutedges,newwalldomains,newvarsaffectedatwall)
    wallinfo = pp.truncateExtendedWallGraph(booleanoutedges,newoutedges,wallinfo)
    print set(wallinfo.keys())==set([(0,4),(0,5),(0,6),(1,0),(2,7),(3,1),(3,2),(4,7),(5,8),(6,11),(6,12),(7,13),(8,9),(8,10),(9,3),(10,7),(11,13),(12,14),(12,15),(13,16),(14,8),(15,16),(16,9),(16,10)])
    print set(wallinfo[(1,0)])==set( [ (4,('umu',)), (5,('umu',)), (6,('umu',))] )
    print wallinfo[(0,4)]==[(7,('uuM',))]
    print wallinfo[(0,5)]==[(8,('Muu',))]
    print set(wallinfo[(0,6)])==set([(11,('uuu',)),(12,('uuu',))])
    print wallinfo[(2,7)]==[(13,('uum',))]
    print wallinfo[(3,1)]==[(0,('udm',))]
    print wallinfo[(3,2)]==[(7,('umd',))]
    print wallinfo[(4,7)]==[(13,('uum',))]
    print set(wallinfo[(5,8)])==set([(9,('duM',)),(10,('duM',))])
    print wallinfo[(6,11)]==[(13,('uuu',))]
    print set(wallinfo[(6,12)])==set([(14,('Muu',)),(15,('Muu',))])
    print wallinfo[(7,13)]==[(16,('Muu',))]
    print wallinfo[(8,9)]==[(3,('dMd',))]
    print wallinfo[(8,10)]==[(7,('mud',))]
    print set(wallinfo[(9,3)])==set([(1,('mdd',)),(2,('mdd',))])
    print wallinfo[(10,7)]==[(13,('uum',))]
    print wallinfo[(11,13)]==[(16,('Muu',))]
    print wallinfo[(12,14)]==[(8,('duu',))]
    print wallinfo[(12,15)]==[(16,('duu',))]
    print set(wallinfo[(13,16)])==set([(9,('duM',)),(10,('duM',))])
    print set(wallinfo[(14,8)])==set([(9,('duM',)),(10,('duM',))])
    print set(wallinfo[(15,16)])==set([(9,('duM',)),(10,('duM',))])
    print wallinfo[(16,9)]==[(3,('dMd',))]
    print wallinfo[(16,10)]==[(7,('mud',))]




if __name__=='__main__':
    testme()
