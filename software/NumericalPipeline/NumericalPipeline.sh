#!/bin/bash

if [ "$#" -ne 2 ]; then
    echo "Usage: NumericalPipeline data_folder DSGRN_folder"
    echo "note: The DSGRN folder is required to find scripts/binaries"
    exit 1
fi


data_folder=$1
DSGRN=$2
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
./scripts/CreateSpecFiles.py $json_input $network_spec_file $pattern_spec_file || exit 1

# Run a database
mpiexec -np 9 Signatures $network_spec_file $database_file || exit 1

# Perform SQL Query for StableFC
StableFCQuery.sh $database_file $stable_FC_matches_file || exit 1

# Acquire only parameter indices
cut -d " " -f 1 $stable_FC_matches_file | sort | uniq > $parameter_indices_file || exit 1

# Sample Parameters for each parameter index
cat $parameter_indices_file | ParameterSampler $network_spec_file > $parameter_list_file || exit 1

# Create simulation database for sampled parameters
SimulationDatabase $network_spec_file $parameter_list_file $simulation_database_file || exit 1

# Filter out non-matching entries in simulation database
mpiexec -np 9 NumericalSimulationPatternMatch $network_spec_file $pattern_spec_file $simulation_database_file $matched_simulation_database_file || exit 1
