#!/bin/bash
# StableFCQuery input_database output_file
#   "input_database" is the name of a DSGRN database
#   "output_file" is the name of the output file to be produced
#      It will hold a list of parameter index / Morse set index pairs
#      separated by newlines corresponding to detected Morse sets
#      which are minimal in their Morse graph and have the FC
#      annotation.
sqlite3 -separator " " $1 'select ParameterIndex, Vertex from Signatures natural join (select MorseGraphIndex,Vertex from (select MorseGraphIndex,Vertex from MorseGraphAnnotations where Label="FC" except select MorseGraphIndex,Source from MorseGraphEdges));' > $2
