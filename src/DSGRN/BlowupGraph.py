### BlowUpGraph.py
### Shaun Harker
### 2018-03-25
### MIT LICENSE

from pychomp import *
import datetime
from itertools import product
from math import log2
from DSGRN._dsgrn import *

class BlowupGraph:

    def complex(self):
        return self.dc
        
    def diagram(self):
        return self.digraph.adjacencies

    def absorbing(self, cctopcell, d, direction):
        # unfortunately the cc topcell indexing is not compatible with labelling indexing
        # due to the extra wrapped layer
        coords = self.cc.coordinates(cctopcell)
        # Wrap layer boxes report all walls absorbing
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

    def __init__(self, p, logging = None):
        # Algorithm.
        #   For each edge in order complex, 
        #     Let x be the corresponding blow-up complex wall.
        #     Determine the set S of domains which intersect x.
        #     Determine the multi-valued set of directions in those domains near x
        #     Determine if there is transversality; if so make a single edge in the appropriate direction
        #     Otherwise, make a double edge.

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

        ## Debug
        if logging:
            print(datetime.datetime.now().time(), flush=True)
            print("Complex size = " + str(self.cc.size()), flush=True)
            print("Number of top cells in cubical complex = " + str(self.cc.size(self.D)), flush=True)
        for cell in self.dc(self.D):
            self.digraph.add_vertex(cell)
        if logging:
            print(datetime.datetime.now().time(), flush=True)
            print("Graph vertices constructed", flush=True)

        def closedrightfringe(i):
            return any(self.cc.rightfringe(k) for k in self.cc.star({i}))

        def blowupfringe(i):
            """
            determine if cell i is in the fringe
            """
            return all(closedrightfringe(k) for k in self.oc.simplex(self.dc.dual(i)))

        for edge in self.oc(1):
            #print("Line 87. edge = " + str(edge))
            (s,t) = self.oc.simplex(edge)
            S = self.dc.dual(s)
            T = self.dc.dual(t)
            flag = False
            if blowupfringe(S):
                self.digraph.add_edge(S,T)
                flag = True
            if blowupfringe(T):
                self.digraph.add_edge(T,S)    
                flag = True
            if flag:
                continue            
            #print("Line 91. (s,t,S,T) = " + str((s,t,S,T)))
            # s is the index of the lower dimensional cell in the cubical complex self.cc
            # t is the index of the higher dimensional cell in the cubical complex self.cc
            # (s,t) corresponds to an edge in the order complex, which is a wall in the dual-order complex.
            #print("topstar = " + str(self.cc.topstar(t)))
            flowdata = [self.flowdirection(cctopcell,s,t) for cctopcell in self.cc.topstar(t) ]
            #print("Line 96. flowdata = " + str(flowdata))
            if any ( k != 1 for k in flowdata ):
                #print("adding edge " + str((T,S)))
                self.digraph.add_edge(T, S)
            if any ( k != -1 for k in flowdata ):
                #print("adding edge " + str((S,T)))
                self.digraph.add_edge(S, T)
