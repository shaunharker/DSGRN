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

## SimulationDatabase

In `./example/example.sh` there is an example creating a Simulation database.

It takes as inputs a network specification file and a list of parameters of the format
```javascript
{"ParameterIndex":...,"Parameter":{"var":val, "var":val, ...}}
{"ParameterIndex":...,"Parameter":{"var":val, "var":val, ...}}
{"ParameterIndex":...,"Parameter":{"var":val, "var":val, ...}}
```

It outputs a simulation database which has a one-to-one correspondence with the parameter input list
{"ParameterIndex": 1617, "SimulationSummary": [31, 279, 310, 434, 930, 992, 744, 713, 589, 93, 31, 279, 310, 434, 930, 992, 744, 713, 589, 93, 31, 279, 310, 434, 930, 992, 744, 713, 589, 93, 31, 279, 310, 434, 930, 992, 744, 713, 589, 93, 31, 279, 310]}

The SimulationSummary list is a list of "labels" which gives the qualitative behavior of a numerical ODE solution describing consecutive time intervals in the domain of the ODE solution for which each variable experiences monotonic behavior.

Each individual "label" is a binary encoded number where the first D bits have 1's for variables which are decreasing and the next D bits have 1's for variables which are increasing, as in the following:
```python
#  2*D bits, the ith bit is 1 if the final state is decreasing in variable i
#        the (i+D)th bit is 1 if the final state is increasing in variable i
label = 0b10100000101111   # variables 0,1,2,3,5 decreasing, variables 4,6 increasing
```
Here D is the number of network nodes in the network.

No two consecutive labels may be the same (they would be merged together)


