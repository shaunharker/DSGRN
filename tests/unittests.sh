#!/bin/bash
DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
cd $DIR
var=0
while read p; do
  eval $p > /dev/null 
  if [ ! $? -eq 0 ]; then
    echo FAIL: $p
    var=$((var+1))
  fi
done < unittests.txt
if [[ "$var" -eq "0" ]]; then
  exit 0
else
  exit 1
fi
