#!/bin/bash
#Active comments for SGE
#$ -V
#$ -cwd
#$ -j y
#$ -S /bin/bash
#$ -pe orte 8

mpiexec --mca mpi_preconnect_mpi 1 -np $NSLOTS -x LD_LIBRARY_PATH ./bin/Signatures $1.txt ./databases/$1.db