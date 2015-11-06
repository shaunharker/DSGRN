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

import subprocess

def test0():
    # X : X(~Z) : X Z
    # Z : X : X
    # NO STEADY STATES, SEE NOTES FOR FLOW ACROSS WALLS
    walldomains=[(0.5,1),(1,0.5),(1,1.5),(1.5,1),(2,0.5),(2,1.5),(2.5,1)]
    outedges=[(1,),(4,),(0,),(4,),(6,),(2,3),(5,)]
    varsaffectedatwall=[0]*len(outedges)
    for k in [4,5]:
        varsaffectedatwall[k]=1
    return outedges,walldomains,varsaffectedatwall

def test1():
    # X : X(~Z) : X Z
    # Z : X : X
    # HAS STEADY STATE AND WHITE WALL, SEE NOTES FOR FLOW ACROSS WALLS
    walldomains=[(1.5,1),(2,0.5),(2,1.5),(2.5,1)]
    outedges=[(1,),(3,),(0,),(2,)]
    varsaffectedatwall=[-1]*len(outedges)
    for k in [0,3]:
        varsaffectedatwall[k]=0
    for k in [1,2]:
        varsaffectedatwall[k]=1
    return outedges,walldomains,varsaffectedatwall

def test2():
    # X : X(~Z) : Z X
    # Z : X : X
    # NO STEADY STATES, SEE NOTES FOR FLOW ACROSS WALLS
    walldomains=[(0.5,1),(1,0.5),(1,1.5),(1.5,1),(2,0.5),(2,1.5),(2.5,1)]
    outedges=[(1,),(4,3),(0,),(2,),(6,),(2,),(5,)]
    varsaffectedatwall=[-1]*len(outedges)
    for k in [0,3,4,5,6]:
        varsaffectedatwall[k]=0
    for k in [1,2]:
        varsaffectedatwall[k]=1
    return outedges,walldomains,varsaffectedatwall

def test3():
    # X : ~Z : Y
    # Y : ~X : Z
    # Z : ~Y : X
    # 3D EXAMPLE, NEGATIVE FEEDBACK, NO STEADY STATES (ONLY SADDLES)
    domaingraph=[(1,3,5),(2,),(3,),(4,),(5,),(6,),(1,),(2,4,6)]
    domaincells=[
    [[0,1],[0,1],[0,1]],
    [[1,2],[0,1],[0,1]],
    [[1,2],[0,1],[1,2]],
    [[0,1],[0,1],[1,2]],
    [[0,1],[1,2],[1,2]],
    [[0,1],[1,2],[0,1]],
    [[1,2],[1,2],[0,1]],
    [[1,2],[1,2],[1,2]]
    ]
    morseset=domaincells[1:-1]
    vertexmap=range(1,7)
    walldomains=[(1.5,0.5,1),(1,0.5,1.5),(0.5,1,1.5),(0.5,1.5,1),(1,1.5,0.5),(1.5,1,0.5)]
    outedges=[(1,),(2,),(3,),(4,),(5,),(0,)]
    varsaffectedatwall=[0,1,2,0,1,2]
    # walldomains=[(1.5,1,0.5),(1,1.5,0.5),(1.5,0.5,1),(0.5,1.5,1),(1,0.5,1.5),(0.5,1,1.5)]
    # outedges=[(2,),(0,),(4,),(1,),(5,),(3,)]
    # varsaffectedatwall=[2,1,0,0,1,2]
    varnames=['X','Y','Z']
    threshnames=[['Y'],['Z'],['X']]
    f=open('patterns.txt','w')
    f.write('Z min, X max, Y min, Z max, X min, Y max\n Z min, X min, Y max, Y min, Z max, X max')
    f.close()
    return domaingraph,domaincells,morseset,vertexmap,outedges,walldomains,varsaffectedatwall,varnames,threshnames

def test4():
    # X1 : (X1)(~X2) : X1 X2
    # X2 : (X2)(X1) : X2 X1
    # 2D EXAMPLE WITH TWO THRESHOLDS EACH, HAS CYCLES AND 1 OFF FIXED POINT
    domaingraph=[(1,),(2,0),(2,),(0,4),(5,7),(8,),(3,),(6,),(7,)]
    domaincells=[
    [[0,1],[2,3]],
    [[0,1],[1,2]],
    [[0,1],[0,1]],
    [[1,2],[2,3]],
    [[1,2],[1,2]],
    [[1,2],[0,1]],
    [[2,3],[2,3]],
    [[2,3],[1,2]],
    [[2,3],[0,1]]
    ]
    morseset=domaincells[3:]
    vertexmap=range(3,9)
    walldomains=[(1.5,2),(1.5,1),(2,1.5),(2,0.5),(2,2.5),(2.5,2),(2.5,1)]
    outedges=[(1,2),(3,),(5,),(6,),(0,),(4,),(5,)]
    varsaffectedatwall=[0,1,1,1,1,0,1]
    varnames=['X1','X2']
    threshnames=[['X1','X2'],['X2','X1']]
    f=open('patterns.txt','w')
    f.write('X2 max, X1 min, X2 min, X1 max\n X2 max, X2 min, X1 min, X1 max')
    f.close()
    # walldomains=[(1.5,1),(1.5,2),(2,0.5),(2,1.5),(2,2.5),(2.5,1),(2.5,2)]
    # outedges=[(2,),(0,3),(5,),(6,),(1,),(6,),(4,)]
    # varsaffectedatwall=[1,0,1,1,1,1,0]
    return domaingraph,domaincells,morseset,vertexmap,outedges,walldomains,varsaffectedatwall,varnames,threshnames

def test5():
    # X1 : (X2)(~X3) : X2
    # X2 : X1 : X1 X3
    # X3 : X2 : X1
    # 3D EXAMPLE WHERE ONE VAR HAS 2 THRESHOLDS, NO FIXED POINT
    domaingraph=[(3,),(0,4),(1,5,8),(4,),(5,),(11,),(0,),(1,6),(7,),(3,6,10),(4,7,11),(8,)]
    domaincells=[
    [(0,1),(0,1),(0,1)],
    [(0,1),(1,2),(0,1)],
    [(0,1),(2,3),(0,1)],
    [(1,2),(0,1),(0,1)],
    [(1,2),(1,2),(0,1)],
    [(1,2),(2,3),(0,1)],
    [(0,1),(0,1),(1,2)],
    [(0,1),(1,2),(1,2)],
    [(0,1),(2,3),(1,2)],
    [(1,2),(0,1),(1,2)],
    [(1,2),(1,2),(1,2)],
    [(1,2),(2,3),(1,2)],
    ]
    morseset=domaincells[0:2]+domaincells[3:9]+[domaincells[11]]
    vertexmap=[0,1,3,4,5,6,7,8,11]
    walldomains=[(1,0.5,0.5),(0.5,1,0.5),(1,1.5,0.5),(1.5,1,0.5),(1.5,2,0.5),(1.5,2.5,1),(0.5,0.5,1),(0.5,1.5,1),(0.5,1,1.5),(0.5,2,1.5),(1,2.5,1.5)]
    outedges=[(3,),(0,),(4,),(4,),(5,),(10,),(0,),(1,2),(6,),(7,8),(9,)]
    varsaffectedatwall=[1,0,1,0,2,0,0,0,0,2,1]
    f=open('patterns.txt','w')
    f.write('X1 min, X2 min, X3 min, X1 max, X2 max, X3 max\n X1 min, X2 min, X1 max, X3 min, X2 max, X3 max')
    f.close()
    varnames=['X1','X2','X3']
    threshnames=[['X2'],['X1','X3'],['X1']]
    return domaingraph,domaincells,morseset,vertexmap,outedges,walldomains,varsaffectedatwall,varnames,threshnames

def test6():
    # X1 : (X1)(~X3) : X1 X2 X3
    # X2 : X1 : X3
    # X3 : X1(~X2) : X1
    # 3D EXAMPLE WHERE ONE VAR HAS 3 THRESHOLDS, CHOSE PARAM SET WITH FIXED POINT, 2 WHITE WALLS
    domaingraph=[(2,),(0,),(2,),(1,2),(6,8),(4,9),(2,),(3,5,6),(9,10,12),(13,),(6,11),(7,9),(13,14),(15,),(10,15),(11,)]
    domaincells=[
    [(0,1),(0,1),(0,1)],
    [(0,1),(1,2),(0,1)],
    [(0,1),(0,1),(1,2)],
    [(0,1),(1,2),(1,2)],
    [(1,2),(0,1),(0,1)],
    [(1,2),(1,2),(0,1)],
    [(1,2),(0,1),(1,2)],
    [(1,2),(1,2),(1,2)],
    [(2,3),(0,1),(0,1)],
    [(2,3),(1,2),(0,1)],
    [(2,3),(0,1),(1,2)],
    [(2,3),(1,2),(1,2)],
    [(3,4),(0,1),(0,1)],
    [(3,4),(1,2),(0,1)],
    [(3,4),(0,1),(1,2)],
    [(3,4),(1,2),(1,2)],
    ]
    morseset=domaincells[4:6]+domaincells[7:]
    vertexmap=[4,5]+range(7,16)
    walldomains=[
    (2,0.5,0.5),
    (1.5,1,0.5),
    (2,1.5,0.5),
    (1.5,1.5,1),
    (2.5,1,0.5),
    (2.5,0.5,1),
    (3,0.5,0.5),
    (3,1.5,0.5),
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
    outedges=[(4,5,6),(0,),(7,),(1,2),(7,),(8,),(11,12),(13,),(9,10),(3,),(7,),(13,),(14,15),(16,),(8,),(16,),(9,10)]
    varsaffectedatwall=[1,2,1,0,2,0,2,2,2,1,0,2,0,0,2,2,2]
    f=open('patterns.txt','w')
    f.write('X2 min, X1 max, X3 max, X2 max, X1 min, X3 min, X2 min\n X3 max, X3 min, X1 max, X3 max, X2 max, X1 min, X3 min, X2 min\n X1 min, X3 min, X1 max, X3 max\n X2 min, X3 min, X1 max, X3 max, X2 max, X1 min\n X2 min, X3 min, X1 min, X3 max, X2 max, X1 max')
    f.close()
    varnames=['X1','X2','X3']
    threshnames=[['X1','X2','X3'],['X3'],['X1']]
    return domaingraph,domaincells,morseset,vertexmap,outedges,walldomains,varsaffectedatwall,varnames,threshnames

def test7():
    # dsgrn output, repressilator example like test 3
    subprocess.call(["dsgrn network networks/3D_Example.txt domaingraph > dsgrn_domaincells.json"],shell=True)
    subprocess.call(["dsgrn network networks/3D_Example.txt domaingraph json 13 > dsgrn_domaingraph.json"],shell=True)
    subprocess.call(["dsgrn network networks/3D_Example.txt analyze morseset 0 13 >dsgrn_output.json"],shell=True)
    f=open('patterns.txt','w')
    f.write('Z min, X min, Y min, Z max, X max, Y max\n X max, Y max, Z max, X min, Y min, Z min\n X min, Y max, Z min, X max, Y min, Z max\n X max, Y min, Z max, X min, Y max, Z min')
    f.close()

def test8():
    # dsgrn output, 5D Cycle
    subprocess.call(["dsgrn network networks/5D_Cycle.txt domaingraph > dsgrn_domaincells.json"],shell=True)
    subprocess.call(["dsgrn network networks/5D_Cycle.txt domaingraph json 847328 > dsgrn_domaingraph.json"],shell=True)
    subprocess.call(["dsgrn network networks/5D_Cycle.txt analyze morseset 3 847328 >dsgrn_output.json"],shell=True)
    f=open('patterns.txt','w')
    f.write('X3 max, X4 max, X3 min, X4 min\n X3 max, X4 min, X3 min, X4 max')
    f.close()