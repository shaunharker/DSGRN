#!/bin/bash

# NumericalPipeline
#
# Usage: NumericalPipeline data_folder
#
#   Here data_folder is expected to contain a file "NetworkAndPattern.txt" 
#   with contents such as the following example
# {u'MultistableParameterCount': 1, u'StableFCMatchesParameterCount': 7020, u'Network': u'PF3D7_0611200 : (PF3D7_1337100) : E\nPF3D7_1139300 : (~PF3D7_0611200) : E\nPF3D7_1146600 : (~PF3D7_1139300)(~PF3D7_1408200) : E\nPF3D7_1222600 : (PF3D7_1146600)(~PF3D7_1317200) : E\nPF3D7_1317200 : (PF3D7_1408200)(~PF3D7_1356900) : E\nPF3D7_1337100 : (PF3D7_1139300 + PF3D7_1317200) : E\nPF3D7_1356900 : (~PF3D7_1222600) : E\nPF3D7_1408200 : (~PF3D7_0611200)(~PF3D7_1356900) : E\n', u'StableFCParameterCount': 8400, u'ParameterCount': 12544, u'PartialOrder': u'{"dimension": 8, "poset": [[9, 12], [14], [7], [9, 12], [9, 12], [7], [14], [], [7], [11], [7], [1, 6, 13], [11], [14], [8, 2, 10, 5], [9, 12]], "events": [0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7], "label": 45645}'}
#
# TODO: Make options:
#   Hill exponent (default 10)
#   Simulation settings: initial condition (default [1.0, 1.0, ... , 1.0])
#              time of integration (default [0.0, 30.0])
#              timestep (default .01)
#   Number of parameter samples per parameter node (default 10)

if [ "$#" -ne 1 ]; then
    echo "Usage: NumericalPipeline data_folder"
    exit 1
fi


data_folder=$1
DSGRN=`realpath ../../`
PATH=$DSGRN/software/Signatures/bin:$DSGRN/software/PatternMatch/bin:$DSGRN/software/HillSimulations:$DSGRN/software/AlgebraicGeometry/bin:$PATH

old_form_input=$data_folder/NetworkAndPattern.txt
json_input=$data_folder/NetworkAndPattern.json
network_spec_file=$data_folder/network_spec.txt
pattern_spec_file=$data_folder/pattern_spec.json
database_file=$data_folder/database.db
stable_FC_matches_file=$data_folder/StableFC.txt
parameter_indices_file=$data_folder/parameter_indices.txt
parameter_list_file=$data_folder/parameters.json
simulation_database_file=$data_folder/simulation_database.json
matched_simulation_database_file=$data_folder/matched_simulation_database.json


# Convert from python-ish input to expected json input
./scripts/AdjustInput.sh $old_form_input $json_input || exit 1

# Create network spec file and pattern spec files for use by other programs
echo "Extracting network specification and pattern specification from input"
./scripts/CreateSpecFiles.py $json_input $network_spec_file $pattern_spec_file || exit 1

# Run a database
if [ ! -f $database_file ]; then
  echo "Creating DSGRN database for network"
  mpiexec -np 9 Signatures $network_spec_file $database_file || exit 1
fi

# Perform SQL Query for StableFC
echo "Querying for parameter nodes with Stable FC"
StableFCQuery.sh $database_file $stable_FC_matches_file || exit 1

# Acquire only parameter indices
echo "Obtaining corresponding parameter indices"
cut -d " " -f 1 $stable_FC_matches_file | sort | uniq > $parameter_indices_file || exit 1

# Sample 10 Parameters for each parameter index
echo "Sampling parameters"
sed "s/^//g;p;p;p;p;p;p;p;p;p;p" $parameter_indices_file | ParameterSampler $network_spec_file > $parameter_list_file || exit 1

# Create simulation database for sampled parameters
echo "Creating Simulation Database"
rm -f $simulation_database_file
SimulationDatabase $network_spec_file $parameter_list_file $simulation_database_file || exit 1

# Filter out non-matching entries in simulation database
echo "Searching for records in simulation database which match pattern specification"
rm -f $matched_simulation_database_file
mpiexec -np 9 NumericalSimulationPatternMatch $network_spec_file $pattern_spec_file $simulation_database_file $matched_simulation_database_file || exit 1
