### BlowUpGraph.py
### Shaun Harker
### 2018-03-25
### MIT LICENSE

from pychomp import *
import datetime
import operator
import functools
from itertools import product
from math import log2
from DSGRN._dsgrn import *

class BlowupGraph:

    def complex(self):
        return self.dc
        
    def diagram(self):
        return self.digraph.adjacencies

    def closedrightfringe(self,i):
        return any(self.cc.rightfringe(k) for k in self.cc.star({i}))

    def blowupfringe(self,i):
        """
        determine if cell i is in the fringe
        """
        return all(self.closedrightfringe(k) for k in self.oc.simplex(self.dc.dual(i)))

    def NormalVariables(self,s):  # i.e. inessential directions
        """
        return the set of normal variables for a cubical cell s
        """
        shape = self.cc.cell_shape(s)
        return [ d for d in range(0,self.D) if shape & (1 << d) == 0]

    def TangentVariables(self,s):  # i.e. inessential directions
        """
        return the set of normal variables for a cubical cell s
        """
        shape = self.cc.cell_shape(s)
        return [ d for d in range(0,self.D) if shape & (1 << d) != 0]

    def RelativePositionVector(self,s,t):
        """
        Return the relative position vector of two cubical cells;
        this is equal to the difference of combinatorial position of two cells,
        regarding vertices to be at (even,even,...) integer coordinates and
        higher dimensional cells to have odd coordinates for their dimensions with
        extent
        """
        return [ y-x for (x,y) in zip(self.cc.barycenter(s),self.cc.barycenter(t))]

    def absorbing(self, cctopcell, d, direction):
        """
        Return True if the codim 1 face of cctopcell 
          collapsed in dimension d in direction "direction"
          is absorbing
        """
        # The cc topcell indexing is not compatible with labelling indexing
        # due to the extra wrapped layer, so we compute the index idx
        coords = self.cc.coordinates(cctopcell)
        # "Wrap layer" boxes have all walls absorbing.
        if any(coords[i] == self.limits[i] for i in range(0,self.D)):
            return True
        idx = sum (  c * self.pv[i] for (i,c) in enumerate(coords) )
        labelbit = 1 << (d + (self.D if direction == -1 else 0))
        return self.labelling[idx] & labelbit != 0

    def flowdirection(self, cctopcell, s, t):
        """
        Given a cubical domain, and a dual-order wall (s,t) (i.e. s is a face of t in cc) 
        return 1 if a proof of transversality from s->t is known valid in dom
        return -1 if a proof of transversality from t->s is known valid in dom
        return 0 otherwise
        """
        #print("flowdirection" + str((cctopcell, s, t)))
        sshape = self.cc.cell_shape(s)
        scoords = self.cc.coordinates(s)
        tshape = self.cc.cell_shape(t)
        tcoords = self.cc.coordinates(t)
        xorshape = sshape ^ tshape # 1's in positions where s is 0 and t is 1.
        absorbing = False
        repelling = False
        #print("Line 40" + str((sshape, scoords, tshape, tcoords, xorshape)))
        for d in range(0,self.D):
            if xorshape & (1 << d):
                #print("  collapsed dimension " + str(d))
                direction = -1 if scoords[d] > tcoords[d] else 1  # -1 left, 1 right
                #print("  direction " + str(direction))
                #print("  absorbing -> " + str(self.absorbing(cctopcell, d, direction)))
                if self.absorbing(cctopcell, d, direction):
                    absorbing = True
                else:
                    repelling = True
        if absorbing and not repelling:
            return -1
        elif repelling and not absorbing:
            return 1
        else:
            return 0

    def RookField(self,t):
        """
        Given a cubical top-cell t, return the rook field vector
        """
        M = { (True,True): 0, (True,False): -1, (False,True): 1, (False,False) : 0 }
        return [ M[self.absorbing(t, d, -1),self.absorbing(t, d, 1)] for d in range(0,self.D)]

    def Feedback(self,cycle):
        """
        return {  1   if cycle has an even number of repressors
               { -1   otherwise
        """
        edges = list(zip(cycle[:-1],cycle[1:]))+[(cycle[-1],cycle[0])]
        return functools.reduce(operator.mul, [ 1 if self.network.interaction(i,j) else -1 for (i,j) in edges])

    def CrossingNumber(self, q, cycle):
        edges = list(zip(cycle[:-1],cycle[1:]))+[(cycle[-1],cycle[0])]
        return functools.reduce(operator.add, [ 1 if ((1 if self.network.interaction(i,j) else -1) != q[i]*q[j]) else 0 for (i,j) in edges])

    def RegulationMap(self,s):
        """
        Return a dictionary with key-value pairs
        { (k,v) : s has normal variable k which regulates variable v}
        """
        coords = self.cc.coordinates(s)
        # print(self.cc.boxes())
        # print("RegulationMap")
        # print(s)
        # print(self.closedrightfringe(s))
        # print(coords)
        # print(self.NormalVariables(s))
        # for k in self.NormalVariables(s):
        #     print("normal variable")
        #     print(k)
        #     print(self.network.outputs(k))
        #     print(coords[k])
        #     print(self.network.outputs(k)[coords[k]-1])

        return { k : self.network.outputs(k)[coords[k]-1] for k in self.NormalVariables(s)}

    def isOpaque(self,s):  # not definition in paper, check for bug
        """
        Return True if s is an opaque cubical cell,
          i.e. if RegulationMap(s) is a permutation
        """
        rmap = self.RegulationMap(s)
        return set(rmap.keys())==set(rmap.values())

    def isEquilibriumCell(self,s):
        """
        Return True if s is an equilibrium cell
        """
        if not self.isOpaque(s):
            return False
        for t in self.cc.topstar(s):
            rf = self.RookField(t)
            if any(rf[d] != 0 for d in self.TangentVariables(s)):
                return False
        return True

    def EquilibriumCells(self):
        """
        Returning list of equilibrium cells
        """
        return [ s for s in self.cc if (not self.closedrightfringe(s)) and self.isEquilibriumCell(s) ]

    def CycleDecomposition(self,perm):
        """
        Given a key-value map "perm" defining a permutation,
        return a list of cycles, where each cycle is represented 
        as a list of elements
        e.g.  CycleDecomposition({ 1:1, 3:4, 4:3 }) may return
              [[1],[3,4]]  (or any equivalent answer)
        """
        rmap = dict(perm)
        def ExtractCycle(G):
            cycle = list(G.popitem())
            while cycle[-1] in G: 
                cycle.append(G.pop(cycle[-1]))
            return cycle[:-1]
        cycles = []
        while rmap:
            cycles.append(ExtractCycle(rmap))
        return cycles

    def OpacityCycles(self,s):
        """
        Return list of regulatory cycles for an opaque cubical cell s
        """
        return self.CycleDecomposition(self.RegulationMap(s))


    def Rule1(self):
        """
        Produce all edges, except for edges from real cells into wrap cells
        """
        for edge in self.oc(1):
            (s,t) = self.oc.simplex(edge)
            S = self.dc.dual(s)
            T = self.dc.dual(t)
            if self.blowupfringe(S) or not self.blowupfringe(T):
                self.digraph.add_edge(S,T)
            if self.blowupfringe(T) or not self.blowupfringe(S):
                self.digraph.add_edge(T,S) 

    def Rule2(self):
        """
        Remove edges for which there are transversality theorems from Rule 2
        """
        for edge in self.oc(1):
            #print("Line 87. edge = " + str(edge))
            (s,t) = self.oc.simplex(edge)
            S = self.dc.dual(s)
            T = self.dc.dual(t)
            if self.blowupfringe(S) or self.blowupfringe(T):
                continue
            #print("Line 91. (s,t,S,T) = " + str((s,t,S,T)))
            # s is the index of the lower dimensional cell in the cubical complex self.cc
            # t is the index of the higher dimensional cell in the cubical complex self.cc
            # (s,t) corresponds to an edge in the order complex, which is a wall in the dual-order complex.
            #print("topstar = " + str(self.cc.topstar(t)))
            flowdata = [self.flowdirection(cctopcell,s,t) for cctopcell in self.cc.topstar(t) ]
            #print("Line 96. flowdata = " + str(flowdata))
            if all ( k == 1 for k in flowdata ):
                #print("removing edge " + str((T,S)))
                self.digraph.remove_edge(T, S)
            # else:
            #     print((t,s))
            #     #print("      Edge(" + str(self.cc.barycenter(t)) + ", " + str(self.cc.barycenter(s)) + ")")

            if all ( k == -1 for k in flowdata ):
                #print("removing edge " + str((S,T)))
                self.digraph.remove_edge(S, T)
            # else:
            #     print((s,t))



    def Rule3(self):
        """
        Apply Rule 3 edge removals
        """
        print("** Rule 3 ********************************************")
        for s in self.EquilibriumCells():
            print("=====================")
            print("Checking equilibrium cell s = " + str(s))
            print("  topstar(s) = " + str(self.cc.topstar(s)))
            print("  star(s) = " + str(self.cc.star({s})))
            print("  fringe in star: " + str([(k, str(self.cc.coordinates(k)), self.cc.rightfringe(k)) for k in self.cc.star({s})]))
            print("  dim s = " + str(self.cc.cell_dim(s)))
            print("  Coordinates = " + str(self.cc.coordinates(s)))
            print("  Barycenter = " + str(self.cc.barycenter(s)))
            print("  NormalVariables = " + str(self.NormalVariables(s)))
            print("  TangentVariables = " + str(self.TangentVariables(s)))
            print("  RegulationMap = " + str(self.RegulationMap(s)))
            print("  OpacityCycles = " + str(self.OpacityCycles(s)))
            cycles = self.OpacityCycles(s)
            for t in self.cc.topstar(s):
                print("  Checking top cell t = " + str(t))
                q = self.RelativePositionVector(s,t)
                print("    RelativePositionVector" + str((s,t)) + " = " + str(q))
                unstable = [ (self.CrossingNumber(q,cycle) <= 
                    (len(cycle)+self.Feedback(cycle))/2 + 1) for cycle in cycles]
                for cycle in cycles:
                    edges = list(zip(cycle[:-1],cycle[1:]))+[(cycle[-1],cycle[0])]
                    print("    Cycle = " + str(cycle) )
                    print("      len(cycle) = " + str(len(cycle)))
                    print("      edges = " + str(edges))
                    print("      feedback type = " + str(self.Feedback(cycle)))
                    print("      crossing number = " + str(self.CrossingNumber(q,cycle)))
                    print("      calc = " + str([ (1 if ((1 if self.network.interaction(i,j) else -1) != q[i]*q[j]) else 0,"delta[i,j]=" + str((1 if self.network.interaction(i,j) else -1)),"q[i]=" + str(q[i]),"q[j] = " + str(q[j]),"i = "+str(i),"j = " + str(j)) for (i,j) in edges]))
                print("    Unstable = " + str(unstable))
                S = self.dc.dual(s)
                T = self.dc.dual(t)
                if all(unstable):
                    if T in self.digraph.adjacencies(S):
                        self.digraph.remove_edge(T,S)
                        print("    Removing edge from " + str(t) + " to " + str(s))
                        print("      barycenter(" + str(t) + ") = " + str(self.cc.barycenter(t)))
                        print("      barycenter(" + str(s) + ") = " + str(self.cc.barycenter(s)))
                        print("      CUT(" + str(self.cc.barycenter(t)) + ", " + str(self.cc.barycenter(s)) + ")")
          
                if not any(unstable):
                    if S in self.digraph.adjacencies(T):
                        self.digraph.remove_edge(S,T)
                        print("    Removing edge from " + str(s) + " to " + str(t))
                        print("      barycenter(" + str(s) + ") = " + str(self.cc.barycenter(s)))
                        print("      barycenter(" + str(t) + ") = " + str(self.cc.barycenter(t)))
                        print("      CUT(" + str(self.cc.barycenter(s)) + ", " + str(self.cc.barycenter(t)) + ")")

    def Rule4(self):
        """
        Apply Rule 4 edge removals
        """
        print("** Rule 4 ********************************************")
        for s in self.EquilibriumCells():
            cycles = self.OpacityCycles(s)
            for t in self.cc.star({s}):
                q = self.RelativePositionVector(s,t)
                print("    RelativePositionVector" + str((s,t)) + " = " + str(q))
                if any([[q[i] for i in cycle].count(0) > 1 for cycle in cycles]):
                    continue
                qplus = [ x if x != 0 else 1 for x in q ]
                qminus = [ x if x != 0 else -1 for x in q ]           
                unstableplus = [ (self.CrossingNumber(qplus,cycle) <= 
                    (len(cycle)+self.Feedback(cycle))/2 + 1) for cycle in cycles] 
                unstableminus = [ (self.CrossingNumber(qminus,cycle) <= 
                    (len(cycle)+self.Feedback(cycle))/2 + 1) for cycle in cycles]
                unstable = unstableplus + unstableminus
                S = self.dc.dual(s)
                T = self.dc.dual(t)
                if all(unstable):
                    if T in self.digraph.adjacencies(S):
                        self.digraph.remove_edge(T,S)
                if not any(unstable):
                    if S in self.digraph.adjacencies(T):
                        self.digraph.remove_edge(S,T)


    def Rule5(self):
        """
        Apply Rule 5 edge removals
        """
        print("** Rule 5 *#******************************************")
        print(self.EquilibriumCells())
        for s in self.EquilibriumCells():
            print("=====================")
            print("Checking equilibrium cell s = " + str(s))
            print("  topstar(s) = " + str(self.cc.topstar(s)))
            print("  star(s) = " + str(self.cc.star({s})))
            print("  fringe in star: " + str([(k, str(self.cc.coordinates(k)), self.cc.rightfringe(k)) for k in self.cc.star({s})]))
            print("  dim s = " + str(self.cc.cell_dim(s)))
            print("  Coordinates = " + str(self.cc.coordinates(s)))
            print("  Barycenter = " + str(self.cc.barycenter(s)))
            print("  NormalVariables = " + str(self.NormalVariables(s)))
            print("  TangentVariables = " + str(self.TangentVariables(s)))
            print("  RegulationMap = " + str(self.RegulationMap(s)))
            print("  OpacityCycles = " + str(self.OpacityCycles(s)))
            cycles = self.OpacityCycles(s)
            for t in self.cc.star({s}):
                print("  Checking top cell t = " + str(t))
                q = self.RelativePositionVector(s,t)
                print("    RelativePositionVector" + str((s,t)) + " = " + str(q))
                for cycle in cycles:  # debug
                    edges = list(zip(cycle[:-1],cycle[1:]))+[(cycle[-1],cycle[0])]
                    print("    Cycle = " + str(cycle) )
                    print("      len(cycle) = " + str(len(cycle)))
                    print("      edges = " + str(edges))
                    print("      feedback type = " + str(self.Feedback(cycle)))
                    print("      crossing number = " + str(self.CrossingNumber(q,cycle)))
                    print("      calc = " + str([ (1 if ((1 if self.network.interaction(i,j) else -1) != q[i]*q[j]) else 0,"delta[i,j]=" + str((1 if self.network.interaction(i,j) else -1)),"q[i]=" + str(q[i]),"q[j] = " + str(q[j]),"i = "+str(i),"j = " + str(j)) for (i,j) in edges]))
                #print("    Unstable = " + str(unstable)) #debug end

                # skip if not order 2 coface
                if any([[q[i] for i in cycle].count(0) > 1 for cycle in cycles]):
                    continue
                qplus = [ x if x != 0 else 1 for x in q ]
                qminus = [ x if x != 0 else -1 for x in q ]   
                def stability(cycle):
                    unstab1 = (self.CrossingNumber(qplus,cycle) <= 
                        (len(cycle)+self.Feedback(cycle))/2 + 1)
                    unstab2 = (self.CrossingNumber(qminus,cycle) <= 
                        (len(cycle)+self.Feedback(cycle))/2 + 1)
                    print("    Analyzing w_k for cycle " + str(cycle))
                    print("        qplus = " + str(qplus))
                    print("        qminus = " + str(qminus))
                    print("        qplus crossing = " + str(self.CrossingNumber(qplus,cycle)))
                    print("        qminus crossing = " + str(self.CrossingNumber(qminus,cycle)))
                    print("        unstab1 = " + str(unstab1) + "  unstab2 = " + str(unstab2))
                    if unstab1 and unstab2:
                        return 1
                    elif (not unstab1 and not unstab2):
                        return -1
                    else:
                        return 0
                w = [ stability(cycle) for cycle in cycles ]
                print("    w = " + str(w))
                S = self.dc.dual(s)
                T = self.dc.dual(t)
                if all( x <= 0 for x in w) and any(x < 0 for x in w):
                    if S in self.digraph.adjacencies(T):
                        self.digraph.remove_edge(S,T)
                        print("    Removing edge from " + str(s) + " to " + str(t))
                        print("      barycenter(" + str(s) + ") = " + str(self.cc.barycenter(s)))
                        print("      barycenter(" + str(t) + ") = " + str(self.cc.barycenter(t)))
                        print("      CUT(" + str(self.cc.barycenter(s)) + ", " + str(self.cc.barycenter(t)) + ")")

                if all( x >= 0 for x in w) and any(x > 0 for x in w):
                    if T in self.digraph.adjacencies(S):
                        self.digraph.remove_edge(T,S)
                        print("    Removing edge from " + str(t) + " to " + str(s))
                        print("      barycenter(" + str(t) + ") = " + str(self.cc.barycenter(t)))
                        print("      barycenter(" + str(s) + ") = " + str(self.cc.barycenter(s)))
                        print("      CUT(" + str(self.cc.barycenter(t)) + ", " + str(self.cc.barycenter(s)) + ")")
          
    def Rule5b(self):
        """
        Apply Rule 5b edge removals
        """
        print("** Rule 5b *#******************************************")
        print(self.EquilibriumCells())
        for s in self.EquilibriumCells():
            print("=====================")
            print("Checking equilibrium cell s = " + str(s))
            print("  topstar(s) = " + str(self.cc.topstar(s)))
            print("  star(s) = " + str(self.cc.star({s})))
            print("  fringe in star: " + str([(k, str(self.cc.coordinates(k)), self.cc.rightfringe(k)) for k in self.cc.star({s})]))
            print("  dim s = " + str(self.cc.cell_dim(s)))
            print("  Coordinates = " + str(self.cc.coordinates(s)))
            print("  Barycenter = " + str(self.cc.barycenter(s)))
            print("  NormalVariables = " + str(self.NormalVariables(s)))
            print("  TangentVariables = " + str(self.TangentVariables(s)))
            print("  RegulationMap = " + str(self.RegulationMap(s)))
            print("  OpacityCycles = " + str(self.OpacityCycles(s)))
            cycles = self.OpacityCycles(s)
            for t in self.cc.star({s}):
                print("  Checking top cell t = " + str(t))
                q = self.RelativePositionVector(s,t)
                print("    RelativePositionVector" + str((s,t)) + " = " + str(q))
                for cycle in cycles:  # debug
                    edges = list(zip(cycle[:-1],cycle[1:]))+[(cycle[-1],cycle[0])]
                    print("    Cycle = " + str(cycle) )
                    print("      len(cycle) = " + str(len(cycle)))
                    print("      edges = " + str(edges))
                    print("      feedback type = " + str(self.Feedback(cycle)))
                    print("      crossing number = " + str(self.CrossingNumber(q,cycle)))
                    print("      calc = " + str([ (1 if ((1 if self.network.interaction(i,j) else -1) != q[i]*q[j]) else 0,"delta[i,j]=" + str((1 if self.network.interaction(i,j) else -1)),"q[i]=" + str(q[i]),"q[j] = " + str(q[j]),"i = "+str(i),"j = " + str(j)) for (i,j) in edges]))
                #print("    Unstable = " + str(unstable)) #debug end

                # skip if not order 2 coface
                if any([[q[i] for i in cycle].count(0) > 1 for cycle in cycles]):
                    continue
                qplus = [ x if x != 0 else 1 for x in q ]
                qminus = [ x if x != 0 else -1 for x in q ]   
                def stability(cycle):
                    unstab1 = (self.CrossingNumber(qplus,cycle) <= 
                        (len(cycle)+self.Feedback(cycle))/2 + 1)
                    unstab2 = (self.CrossingNumber(qminus,cycle) <= 
                        (len(cycle)+self.Feedback(cycle))/2 + 1)
                    print("    Analyzing w_k for cycle " + str(cycle))
                    print("        qplus = " + str(qplus))
                    print("        qminus = " + str(qminus))
                    print("        qplus crossing = " + str(self.CrossingNumber(qplus,cycle)))
                    print("        qminus crossing = " + str(self.CrossingNumber(qminus,cycle)))
                    print("        unstab1 = " + str(unstab1) + "  unstab2 = " + str(unstab2))
                    if unstab1 and unstab2:
                        return 1
                    elif (not unstab1 and not unstab2):
                        return -1
                    else:
                        return 0
                w = [ stability(cycle) for cycle in cycles ]
                print("    w = " + str(w))
                S = self.dc.dual(s)
                T = self.dc.dual(t)
                if all( x <= 0 for x in w) and any(x < 0 for x in w):
                    if S in self.digraph.adjacencies(T):
                        self.digraph.remove_edge(S,T)
                        print("    Removing edge from " + str(s) + " to " + str(t))
                        print("      barycenter(" + str(s) + ") = " + str(self.cc.barycenter(s)))
                        print("      barycenter(" + str(t) + ") = " + str(self.cc.barycenter(t)))
                        print("      CUT(" + str(self.cc.barycenter(s)) + ", " + str(self.cc.barycenter(t)) + ")")

                if all( x >= 0 for x in w) and any(x > 0 for x in w):
                    if T in self.digraph.adjacencies(S):
                        self.digraph.remove_edge(T,S)
                        print("    Removing edge from " + str(t) + " to " + str(s))
                        print("      barycenter(" + str(t) + ") = " + str(self.cc.barycenter(t)))
                        print("      barycenter(" + str(s) + ") = " + str(self.cc.barycenter(s)))
                        print("      CUT(" + str(self.cc.barycenter(t)) + ", " + str(self.cc.barycenter(s)) + ")")
          
    def Rule6(self):
        """
        Apply Rule 6 edge removals
        """
        print("** Rule 6 ********************************************")
        print(self.EquilibriumCells())
        for s in self.EquilibriumCells():
            print("=====================")
            print("Checking equilibrium cell s = " + str(s))
            print("  topstar(s) = " + str(self.cc.topstar(s)))
            print("  star(s) = " + str(self.cc.star({s})))
            print("  fringe in star: " + str([(k, str(self.cc.coordinates(k)), self.cc.rightfringe(k)) for k in self.cc.star({s})]))
            print("  dim s = " + str(self.cc.cell_dim(s)))
            print("  Coordinates = " + str(self.cc.coordinates(s)))
            print("  Barycenter = " + str(self.cc.barycenter(s)))
            print("  NormalVariables = " + str(self.NormalVariables(s)))
            print("  TangentVariables = " + str(self.TangentVariables(s)))
            print("  RegulationMap = " + str(self.RegulationMap(s)))
            print("  OpacityCycles = " + str(self.OpacityCycles(s)))
            cycles = self.OpacityCycles(s)
            for t in self.cc.star({s}):
                print("  Checking top cell t = " + str(t))
                q = self.RelativePositionVector(s,t)
                print("    RelativePositionVector" + str((s,t)) + " = " + str(q))
                for cycle in cycles:  # debug
                    edges = list(zip(cycle[:-1],cycle[1:]))+[(cycle[-1],cycle[0])]
                    print("    Cycle = " + str(cycle) )
                    print("      len(cycle) = " + str(len(cycle)))
                    print("      edges = " + str(edges))
                    print("      feedback type = " + str(self.Feedback(cycle)))
                    print("      crossing number = " + str(self.CrossingNumber(q,cycle)))
                    print("      calc = " + str([ (1 if ((1 if self.network.interaction(i,j) else -1) != q[i]*q[j]) else 0,"delta[i,j]=" + str((1 if self.network.interaction(i,j) else -1)),"q[i]=" + str(q[i]),"q[j] = " + str(q[j]),"i = "+str(i),"j = " + str(j)) for (i,j) in edges]))
                #print("    Unstable = " + str(unstable)) #debug end

                # # skip if not order 2 coface
                # if any([[q[i] for i in cycle].count(0) > 1 for cycle in cycles]):
                #     continue
                qplus = [ x if x != 0 else 1 for x in q ]
                qminus = [ x if x != 0 else -1 for x in q ]   
                def stability(cycle):
                    unstab1 = (self.CrossingNumber(qplus,cycle) <= 
                        (len(cycle)+self.Feedback(cycle))/2 + 1)
                    unstab2 = (self.CrossingNumber(qminus,cycle) <= 
                        (len(cycle)+self.Feedback(cycle))/2 + 1)
                    print("    Analyzing w_k for cycle " + str(cycle))
                    print("        qplus = " + str(qplus))
                    print("        qminus = " + str(qminus))
                    print("        qplus crossing = " + str(self.CrossingNumber(qplus,cycle)))
                    print("        qminus crossing = " + str(self.CrossingNumber(qminus,cycle)))
                    print("        unstab1 = " + str(unstab1) + "  unstab2 = " + str(unstab2))
                    if unstab1 and unstab2:
                        return 1
                    elif (not unstab1 and not unstab2):
                        return -1
                    else:
                        return 0
                w = [ stability(cycle) for cycle in cycles ]
                print("    w = " + str(w))
                S = self.dc.dual(s)
                T = self.dc.dual(t)
                if all( x <= 0 for x in w) and any(x < 0 for x in w):
                    if S in self.digraph.adjacencies(T):
                        self.digraph.remove_edge(S,T)
                        print("    Removing edge from " + str(s) + " to " + str(t))
                        print("      barycenter(" + str(s) + ") = " + str(self.cc.barycenter(s)))
                        print("      barycenter(" + str(t) + ") = " + str(self.cc.barycenter(t)))
                        print("      CUT(" + str(self.cc.barycenter(s)) + ", " + str(self.cc.barycenter(t)) + ")")

                if all( x >= 0 for x in w): # and any(x > 0 for x in w):
                    if T in self.digraph.adjacencies(S):
                        self.digraph.remove_edge(T,S)
                        print("    Removing edge from " + str(t) + " to " + str(s))
                        print("      barycenter(" + str(t) + ") = " + str(self.cc.barycenter(t)))
                        print("      barycenter(" + str(s) + ") = " + str(self.cc.barycenter(s)))
                        print("      CUT(" + str(self.cc.barycenter(t)) + ", " + str(self.cc.barycenter(s)) + ")")
          

    def __init__(self, p, level = 0):
        # initialize complex, graph, supporting data structures
        self.p = p
        self.network = self.p.network()
        self.D = self.network.size()
        self.cc = CubicalComplex([ x + 1 for x in self.network.domains()])
        self.digraph = DirectedAcyclicGraph()
        self.labelling = p.labelling()
        self.limits = self.network.domains()
        self.pv = [1]
        for i in self.limits:
            self.pv.append(self.pv[-1]*i)
        self.oc = OrderComplex(self.cc)
        self.dc = DualComplex(self.oc)
        for cell in self.dc(self.D):
            self.digraph.add_vertex(cell)

        # Apply rules
        self.Rule1()
        self.Rule2()
        if level == 3:
            self.Rule3()
        if level == 4:
            self.Rule3()
            self.Rule4()
        if level == 5:
            self.Rule3()
            self.Rule5()
        if level == "5b":
            self.Rule3()
            self.Rule5b()
        if level == 6:
            self.Rule3()
            self.Rule6()
