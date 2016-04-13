# Hill Function Simulations

The main script to run is runmany2b.sh. 

Currently runmany2b.sh breaks the process down into sections: 
 1. first, it finds all relevant parameters, 
 2. and then it finds the CAD solutions to all the relevant parameter nodes, 
 3. then it runs the script to produce plots and check for oscillations for each parameter node, 
 4. and then it sees which parameter nodes have all genes oscillating have max/min orderings that match the experimental data.

An alternative procedure is to merge those sections into one loop that loops through each parameter and matches its output to experimental data (if all its genes are oscillating) instead of waiting until the end to do that (so it analyzes parameter node 1, outputs if it's a match, then moves onto parameter node 2).