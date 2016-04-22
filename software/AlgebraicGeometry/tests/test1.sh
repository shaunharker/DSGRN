#!/bin/bash
SHELL_DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
cd $SHELL_DIR
cat parameter_indices.txt | ../build/bin/ParameterSampler  network_spec.txt > parameters.json
