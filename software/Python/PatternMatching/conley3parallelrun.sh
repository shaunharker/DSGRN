#!/bin/bash

# Parallel Python SGE script
#  * Change the number after -pe orte to require more cores
#  * Change the name of the script in STEP 2 to change the
#    python program.

# call as qsub -pe orte conley3parallelrun.sh NUMCPUS "useDSGRN.py"

#$ -V
#$ -cwd
#$ -j y
#$ -S /bin/bash

start=`date +%s`

ulimit -n 4096

# STEP 1. Launch Parallel Python on the nodes
echo Launching servers

export PPSERVERSECRET=`cat /dev/urandom | tr -dc 'a-zA-Z0-9' | fold -w 8 | head -n 1`

exec 5< $PE_HOSTFILE
while read nodename corecount queue undef <&5 ; do
    echo "ssh -f $nodename ppserver.py -w $corecount -a -s \"$PPSERVERSECRET\" -t 600"
    ssh -f $nodename "export PYTHONPATH=$PWD:$PYTHONPATH; ppserver.py -w $corecount -a -s \"$PPSERVERSECRET\" -t 600" & 
done
exec 5<&-

# STEP 1.5  Wait for python servers to properly initialize
echo Sleeping for a bit so servers can get ready.
sleep 30s
echo Waking up and running script.

# STEP 2. Run the script
export PYTHONPATH=$PWD:$PYTHONPATH;
python $1

# STEP 3. Kill Parallel Python
echo Ending servers
exec 5< $PE_HOSTFILE
while read nodename corecount queue undef <&5 ; do
    ssh $nodename "killall ppserver.py -u $USER"
done
exec 5<&-

# STEP 4. Print wall time and exit
end=`date +%s`
echo $((end-start))

exit 0
