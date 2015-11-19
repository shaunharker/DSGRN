#!/bin/bash

# Parallel Python SGE script
#  * Change the number after -pe orte to require more cores
#  * Change the name of the script in STEP 2 to change the
#    python program.
#  * Call as qsub -pe orte NumberNodes ThisScript.sh "ExampleProgram.py NumberNodes"


#$ -V
#$ -cwd
#$ -j y
#$ -S /bin/bash


# STEP 1. Launch Parallel Python on the nodes
echo Launching servers
exec 5< $PE_HOSTFILE
while read nodename corecount queue undef <&5 ; do
    echo "ssh -f $nodename ppserver.py -w $corecount -a"
    ssh -f $nodename "export PYTHONPATH=$PWD:$PYTHONPATH; ppserver.py -w $corecount -a" &
    # ssh -f $nodename "ppserver.py -w $corecount -a" & 
done
exec 5<&-

# STEP 1.5  Wait for python servers to properly initialize
echo Sleeping for a bit so servers can get ready.
sleep 30s
echo Waking up and running script.

# STEP 2. Run the script
python $1 

# STEP 3. Kill Parallel Python
echo Ending servers
exec 5< $PE_HOSTFILE
while read nodename corecount queue undef <&5 ; do
    ssh $nodename "killall ppserver.py -u $USER"
done
exec 5<&-

exit 0
