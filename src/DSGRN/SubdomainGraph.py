### SubdomainGraph.py
### Shaun Harker
### 2018-03-25
### MIT LICENSE

from pychomp import *
import datetime
from itertools import product
from math import log2
from DSGRN._dsgrn import *

class SubdomainGraph:

    def complex(self):
        return self.cc

    def diagram(self):
        return self.digraph.adjacencies

    def domains(self):
        def domgen():
            sizes = self.network.domains()
            dom = Domain(sizes)
            while dom.isValid():
                yield dom
                dom.preincrement()
        return domgen()

    def walls(self):
        return ( wall for wall in self.cc(self.D-1) if not self.cc.rightfringe(wall) )

    def box(self,subdom):
        return self.cc.cell_index(subdom, 2**self.D - 1)

    def subdomains(self):
        return ( box for box in self.cc(self.D) if not self.cc.rightfringe(box) )   

    def centerbox(self,dom):
        return self.box([3*i + 1 for i in dom])

    def wallnormal(self,wall):
        shape = self.cc.cell_shape(wall)
        return int(log2(2**self.D - 1 - shape))

    def posterior(self,wall):
        dim = self.wallnormal(wall)
        return self.cc.left(self.cc.left(wall,dim),dim)

    def anterior(self,wall):
        dim = self.wallnormal(wall)
        return self.cc.right(self.cc.right(wall,dim),dim)

    ## labelling

    def leftlabel(self):
        return 1

    def rightlabel(self):
        return 2   

    def isLeftBoundingWall(self, wall):
        normdim = self.wallnormal(wall)
        return self.cc.leftfringe(wall)
    
    def isRightBoundingWall(self, wall):
        normdim = self.wallnormal(wall)
        return self.cc.rightfringe(self.cc.right(wall, normdim))

    def __init__(self, p, logging = None):
        self.p = p
        self.network = self.p.network()
        self.D = self.network.size()
        self.cc = CubicalComplex([ 3*x + 1 for x in self.network.domains()])
        self.digraph = DirectedAcyclicGraph()
        if logging:
            print(datetime.datetime.now().time(), flush=True)
            print("Complex size = " + str(self.cc.size()), flush=True)
            print("Number of top cells = " + str(self.cc.size(self.D)), flush=True)
        for box in self.cc(self.D):
            self.digraph.add_vertex(box, str(self.cc.coordinates(box)))
        if logging:
            print(datetime.datetime.now().time(), flush=True)
            print("Graph vertices constructed", flush=True)

        # construct wall labels

        # Step 1. Center labels
        label = {}
        for dom in self.domains():
            box = self.centerbox(dom)
            for dim in range(0,self.D):
                x = self.p.absorbing(dom, dim, 1)  # 1 -- right
                rightwall = self.cc.right(box, dim)
                label[rightwall] = self.rightlabel() if x else self.leftlabel()
                y = self.p.absorbing(dom, dim, -1)  # -1 -- left
                leftwall = self.cc.left(box, dim)
                label[leftwall] = self.leftlabel() if y else self.rightlabel()

        if logging:
            print(datetime.datetime.now().time(), flush=True)
            print("Computed center labels", flush=True)

        # Step 2. Center Transverse labels
        for dom in self.domains():
            box = self.centerbox(dom)
            for dim in range(0,self.D):
                for wall in self.posterior(self.cc.left(box, dim)), self.anterior(self.cc.right(box, dim)):
                    if self.isLeftBoundingWall(wall):
                        label[wall] = self.rightlabel()
                        continue
                    if self.isRightBoundingWall(wall): 
                        label[wall] = self.leftlabel()
                        continue
                    posteriorlabel = label.get(self.posterior(wall),0)
                    anteriorlabel = label.get(self.anterior(wall),0)
                    if posteriorlabel == anteriorlabel:
                        for adjwall in self.cc.parallelneighbors(wall):
                            label[adjwall] = posteriorlabel

        if logging:
            print(datetime.datetime.now().time(), flush=True)
            print("Computed transverse labels", flush=True)

        # Step 4. Propagate labels
        newlabel = dict(label)
        for wall in label:
            for adjwall in self.cc.parallelneighbors(wall):
                if self.cc.rightfringe(adjwall): continue
                if adjwall not in newlabel:
                    newlabel[adjwall] = 0
                newlabel[adjwall] |= label[wall]
        label = newlabel 

        if logging:
            print(datetime.datetime.now().time(), flush=True)
            print("Computed all wall labels", flush=True)

        # Build graph edges
        for wall in self.cc(self.D-1):
            #print(" wall = " + str(wall),flush=True)
            normdim = self.wallnormal(wall)
            leftbox = self.cc.left(wall, normdim)
            rightbox = self.cc.right(wall, normdim)
            #print(str(self.cc.coordinates(leftbox)) + " and " + str(self.cc.coordinates(rightbox)))
            #print(label.get(wall,0))
            if label.get(wall,0) in [0,3]:
                #print(str(self.cc.coordinates(leftbox)) + " <-> " + str(self.cc.coordinates(rightbox)))
                self.digraph.add_edge(leftbox, rightbox)
                self.digraph.add_edge(rightbox, leftbox)
            elif label[wall] == self.rightlabel():
                #print(str(self.cc.coordinates(leftbox)) + " -> " + str(self.cc.coordinates(rightbox)))
                self.digraph.add_edge(leftbox, rightbox)
            elif label[wall] == self.leftlabel():
                #print(str(self.cc.coordinates(leftbox)) + " <- " + str(self.cc.coordinates(rightbox)))
                self.digraph.add_edge(rightbox, leftbox)
        if logging:
            print(datetime.datetime.now().time(), flush=True)
            print("Constructed SubdomainGraph", flush=True)
