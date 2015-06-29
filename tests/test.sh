#!/bin/bash
DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
cd $DIR
prefix=./suite/$1
infile=${prefix}.in
txtfile=${prefix}.txt
outfile=${prefix}.out
/bin/bash $infile > $txtfile
diff -q $outfile $txtfile > /dev/null
