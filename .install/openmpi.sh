#!/bin/bash
## Parse command line arguments to get install PREFIX
SHELL_DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
source $SHELL_DIR/parse.sh

## Install OpenMPI
wget http://www.open-mpi.org/software/ompi/v1.8/downloads/openmpi-1.8.1.tar.gz || exit 1
tar xvfz openmpi-1.8.1 || exit 1
cd openmpi-1.8.1 
./configure --prefix=${PREFIX} || exit 1
make all install               || exit 1
