#!/bin/bash
SHELL_DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
cd $SHELL_DIR
../SimulationDatabase ./network_spec.txt ./parameters.json ./simulation_database.json
