# AlgebraicDecomposition

Authors: Kafung Mok, Shaun Harker

This subproject of DSGRN has several functions.
(1) It can be used to prove the GPG = CPG result
    for network nodes with fewer than 4 inputs
    (high numbers of inputs currently appear to be
     intractable with current techniques)
(2) It can be used to produce databases of CAD
    descriptions (Cylindrical Algebraic Decompositions)
    used to describe the semialgebraic sets which 
    arise in the analysis of regulatory networks.
(3) A Gibbs sampling algorithm is provided which 
    can sample from semialgebraic sets which describe
    the parameter space regions corresponding to parameter
    nodes. Currently, the Gibbs sampler assume an a-priori 
    distribution X_i ~ Exp(1) for all variables X_i, 
    conditioned on X being within the semialgebraic set.

We refer to 

Cummins, Bree, Tomas Gedeon, Shaun Harker, Konstantin Mischaikow, and Kafung Mok. "Combinatorial Representation of Parameter Space for Switching Systems." arXiv preprint arXiv:1512.04131 (2015)

for more details on the theory behind these programs.

Usage:

TODO
