#!/bin/bash
DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
cd $DIR
prefix=./suite/$1
infile=${prefix}.in
txtfile=${prefix}.txt
outfile=${prefix}.out
source $infile ../build/bin/dsgrn > $txtfile
diff -q $outfile $txtfile > /dev/null
if [ ! $? -eq 0 ]; then
  echo FAIL: $1
  echo   EXPECTED: `cat $outfile`
  echo   GOT: `cat $txtfile`
  rm $txtfile
  exit 1
else
  rm $txtfile
  exit 0
fi
