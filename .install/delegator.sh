#!/bin/bash
## Parse command line arguments to get install PREFIX and MASS
SHELL_DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
source $SHELL_DIR/parse.sh

git clone https://github.com/shaunharker/cluster-delegator.git
cd cluster-delegator
./install.sh --prefix=$PREFIX

