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

import json

def parseMorseGraphs(fname="morsegraphs.txt"):
    f=open(fname,'r')
    morse_graphs_and_sets=[]
    for l in f.readlines():
        L=l.replace('|',' ').split()
        morse_graphs_and_sets.append((L[0],L[1:]))
    f.close()
    return morse_graphs_and_sets

def parseParameters(fname="concatenatedparams.txt"):
    f=open(fname,'r')
    morsegraph_morseset_param=[]
    for l in f.readlines():
        morsegraph_morseset_param.append(tuple(l.split('|')))
    f.close()
    return morsegraph_morseset_param

def parsePatterns(fname="patterns.txt"):
    f=open(fname,'r')
    maxmin=[]
    varnames=[]
    originalpatterns=[]
    for l in f:
        if l[-1]=='\n':
            l=l[:-1]
        originalpatterns.append(l)
        L=l.replace(',',' ').split()
        varnames.append(L[::2])
        maxmin.append(L[1::2])
    f.close()
    return varnames, maxmin, originalpatterns

def parseMorseSet(fname='dsgrn_output.json'):
    parsed = json.load(open(fname),strict=False)
    varnames = [ x[0] for x in parsed["network"] ]
    threshnames = [ [parsed["network"][i][2][j] for j in parsed["parameter"][i][2]] for i in range(len(parsed["network"])) ]
    return varnames,threshnames,parsed["graph"],parsed["cells"],parsed["vertices"]

def parseDomainCells(fname='dsgrn_domaincells.json'):
    parsed = json.load(open(fname),strict=False)
    return parsed["cells"]

def parseDomainGraph(fname="dsgrn_domaingraph.json"):
    return json.load(open(fname),strict=False)


