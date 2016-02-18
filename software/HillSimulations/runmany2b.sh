#!/bin/bash

#change the following section to desired inputs:
networkname="5D_2016_01_28"

resultsfile='/home/mike/conley3_outputs/5D_2016_01_28_stableFCs_allresults.txt'
pnMG='/home/mike/5D_2016_01_28_pnMG.txt'

python make_pnMG.py

parameter_list_location='/home/mike/5D_2016_01_28_parameters.txt'

extrema_location=$networkname'_extrema'

dsgrn network ./networks/$networkname.txt

if [ -f /home/mike/timeseries_outputs/$networkname/$extrema_location.txt ] 
  then
      rm /home/mike/timeseries_outputs/$networkname/$extrema_location.txt
fi

if [ ! -d /home/mike/DSGRN_outputs/new/$networkname/ ]
then
    mkdir -p /home/mike/DSGRN_outputs/new/$networkname
fi

va=$(python parse_conley3_3.py $parameter_list_location 2>&1 | tr -d "[],'") 
declare -a arr=($va)
for num in "${arr[@]}"
do
  #remove because this script writes the output after the file's old contents, instead of overwriting all its old contents
  #This is in contrast to 'file_ = open(samples, 'w')' in python
  if [ -f /home/mike/DSGRN_outputs/new/$networkname/$num.txt ] 
  then
      rm /home/mike/DSGRN_outputs/new/$networkname/$num.txt 
  fi
  dsgrn parameter inequalities $num > $num.ineq
  ./ToMathematicaTest $num >> instances.m
  /usr/local/bin/MathKernel -script instances.m >> /home/mike/DSGRN_outputs/new/$networkname/$num.txt
  rm $num.ineq
  rm instances.m
done

python hillmodelscripts3.py $networkname $pnMG

python compare.py

#INSTRUCTIONS: The only inputs the user needs to change are 'networkname', 'resultsfile' and 'pnMG'. 
#Change path names in the other scripts
#delete extrema files that hillmodelscripts.py outputs before running new since script appends instead of w

#SUMMARY OF SCRIPT: This script uses: parse_conley3_3.py, make_pnMG.py, ToMathematicaTest, hillmodel.py, hillmodelscripts3.py, and compare.py
#Given a network and one of its morse graphs, for
#each parameter node in the morse graph, it outputs a file of parameters (according to CAD region given by DSGRN output) and a plot of the
#ODE simulation. First, it uses parse_conley3.py to find which parameter nodes it should get parameters from. Then it goes through the list 
#of relevant parameter nodes: for each iteration, it obtains the parameters (the DSGRN output). Then it runs hillmodelscripts.py to generate #the plot. Once the plot is saved, it moves on to the next parameter node until all of them have their plots saved. Finally compare.py

#this takes the parameters from resultsfile.txt and runs through them. runmany.sh (the original one) takes parameters from a database and runs
#through them
