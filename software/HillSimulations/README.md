# Hill Function Simulations

Authors: Bree Cummins, Michael Lan, Shaun Harker

Here we provide scripts to create Hill Model ODE models corresponding to DSGRN computations. The scripts

hillmodel.py

provides methods which can do the following:

1. Construct a Hill Model ODE given a network file and a parameter index
2. Construct numerical solutions of the ODE
3. Provide plots of the numerical solution

We provide a wrapper script

RunSimulation

which gives a short command line interface to produce time-series output given a network and a parameter index.

The scripts themselves contain documentation for further details on their use.

FUTURE WORK:
In addition, we provide tools to analyze the numerical solutions to produce qualitative summaries comparable to the information produced by the "PatternMatch" subproject. We also provide tools to compare the qualitative summaries of the ODE solutions to the patterns detected by "PatternMatch"
