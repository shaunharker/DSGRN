# Algebraic Geometry routines for DSGRN

Authors: Kafung Mok, Shaun Harker

This subproject of DSGRN is made to handle algebraic geometry related 
tasks arising in the analysis of parameter space.

We refer to 

Cummins, Bree, Tomas Gedeon, Shaun Harker, Konstantin Mischaikow, and Kafung Mok. "Combinatorial Representation of Parameter Space for Switching Systems." [arXiv preprint arXiv:1512.04131 (2015) ](http://arxiv.org/abs/1512.04131).

for more details on the geometry of parameter space regions studied using DSGRN.

This subproject provides the following features:

1. It can be used to prove the GPG = CPG result
    for network nodes with fewer than 4 inputs
    (high numbers of inputs currently appear to be
     intractable with current techniques)
2. It can be used to produce databases of CAD
    descriptions (Cylindrical Algebraic Decompositions)
    used to describe the semialgebraic sets which 
    arise in the analysis of regulatory networks.
3. A Gibbs sampling algorithm is provided which 
    can sample from semialgebraic sets which describe
    the parameter space regions corresponding to parameter
    nodes. Currently, the Gibbs sampler assume an a-priori 
    distribution X_i ~ Exp(1) for all variables X_i, 
    conditioned on X being within the semialgebraic set.


Usage:

TODO
