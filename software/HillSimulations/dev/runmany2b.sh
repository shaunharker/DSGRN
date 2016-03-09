#!/bin/bash

#change the following section to desired inputs:
#networkname="5D_2016_01_28"

#networkfile='/home/mike/DSGRN-master/networks/5D_2016_01_28.txt'
#resultsfile='/home/mike/conley3_outputs/5D_2016_01_28_stableFCs_allresults.txt'
#savein_location='/home/mike/timeseries_outputs/5D_2016_01_28/'
#parameter_file_path='/home/mike/DSGRN_outputs/new/5D_2016_01_28/'

networkfile=$1
resultsfile=$2
savein_location=$3
parameter_file_path=$4

extrema_file=$savein_location'TimeSeries.txt'
if [ -f $extrema_file ] 
  then
      rm $extrema_file
fi

if [ ! -d $parameter_file_path ]
then
    mkdir -p $parameter_file_path
fi

dsgrn network $networkfile

va=$(python parse_conley3_3.py $resultsfile 2>&1 | tr -d "[],'") 
declare -a arr=($va)
for num in "${arr[@]}"
do
  #remove because this script writes the output after the file's old contents, instead of overwriting all its old contents
  #This is in contrast to 'file_ = open(samples, 'w')' in python
  parameters_file=$parameter_file_path$num.txt 
  if [ -f $parameters_file ] 
  then
      rm $parameters_file
  fi
  dsgrn parameter inequalities $num > $num.ineq
  ./ToMathematicaTest $num >> instances.m
  /usr/local/bin/MathKernel -script instances.m >> $parameters_file
  rm $num.ineq
  rm instances.m
  python hillmodelscripts3.py $num $networkfile $savein_location $parameters_file
done

python compare.py

#INSTRUCTIONS:
#compare: resultsfile,networkfile, extrema_location
#4 total inputs: 1.resultsfile, 2.networkfile, 3.save_location (folder to save extrema_location (save comparison) and savein (save FCs))
#and 4.parameter_file_path (where to save and retrieve files from DSGRN)

#example: ./runmany2b.sh '/home/mike/DSGRN-master/networks/5D_2016_01_28.txt' '/home/mike/conley3_outputs/5D_2016_01_28_stableFCs_allresults.txt' '/home/mike/timeseries_outputs/5D_2016_01_28/' '/home/mike/DSGRN_outputs/new/5D_2016_01_28/'

#SUMMARY OF SCRIPT: This script uses: parse_conley3_3.py, make_pnMG.py, ToMathematicaTest, hillmodel.py, hillmodelscripts3.py, and compare.py
#Given a network and one of its morse graphs, for
#each parameter node in the morse graph, it outputs a file of parameters (according to CAD region given by DSGRN output) and a plot of the
#ODE simulation. First, it uses parse_conley3.py to find which parameter nodes it should get parameters from. Then it goes through the list 
#of relevant parameter nodes: for each iteration, it obtains the parameters (the DSGRN output). Then it runs hillmodelscripts.py to generate #the plot. Once the plot is saved, it moves on to the next parameter node until all of them have their plots saved. Finally compare.py

#this takes the parameters from resultsfile.txt and runs through them. runmany.sh (the original one) takes parameters from a database and runs
#through them