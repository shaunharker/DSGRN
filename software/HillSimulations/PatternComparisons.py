#!/usr/bin/python
# PatternComparisons.py
#
# USAGE: PatternComparisons.py /path/to/data/directory
#
#   This file reads "Simulation_Patterns.txt" and "Stable_FC_PatternMatch.txt" in
#   the specified data directory and outputs a file "PatternComparisons.txt" to the
#   specified data directory. 
#
#   This file contains information about which parameter nodes had simulations reported 
#   in "Simulation_Patterns.txt" for which the measured pattern of extrema matches one of 
#   the patterns predicted in "Stable_FC_PatternMatch.txt". Note that by "match" we mean 
#   any cyclic permutation of the measured extrema pattern.

import sys

def main(argv):
    simulation_input = open(argv[1] + '/Simulation_Patterns.txt', 'r')
    patternmatch_input = open(argv[1] + '/Stable_FC_PatternMatch.txt','r')
    comparison_output = open(argv[1] + '/PatternComparisons.txt','w')
    PatternTable={}
    for line in patternmatch_input:
      #  e.g. line == "Parameter: 17052, Morse Graph: 0, Morse Set: 0, Pattern: 72 max, 199 max, 204 max, 177 max, 77 max, 199 min, 177 min, 77 min, 72 min, 204 min, 72 max"
      parsed = line.replace(',','').split()
      pattern = [ parsed[i] + parsed[i+1] for i in range(9,len(pattern)-2,2) ] # ignore last pair as it is a repeat
      parameterIndex = parsed[1]
      PatternTable.setdefault(parameterIndex,[]).append(pattern)
    for line in simulation_input:
      #  e.g. line == "17052, 72 max, 199 max, 204 max, 177 max, 77 max, 199 min, 177 min, 77 min, 72 min, 204 min"
      parsed = line.replace(',','').split()
      parameterIndex = parsed[0]
      pattern = [ parsed[i] + parsed[i+1] for i in range(1,len(parsed), 2)]
      for matched_pattern in PatternTable[parameterIndex]:
        # Check for match: is matched_pattern a cyclic permutation of pattern?
        N = len(pattern); M = len(matched_pattern); R = xrange(0, N)
        if (N == M) and any(all(pattern[j]==matched_pattern[(i+j)%N] for j in R) for i in R):
          comparison_output.write(parameterIndex+" MATCH " + matched_pattern + "\n")
    simulation_input.close(); patternmatch_input.close(); comparison_output.close()

if __name__ == "__main__":
   main(sys.argv)
