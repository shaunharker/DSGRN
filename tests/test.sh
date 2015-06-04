#!/bin/bash

numtests=0
numpassed=0
for infile in ./suite/*.in ; do
    numtests=$((numtests+1))
    prefix=${infile%.in}
    txtfile=${prefix}.txt
    outfile=${prefix}.out
    /bin/bash $infile > $txtfile
    passed=1
    diff -q $outfile $txtfile > /dev/null
    if [ $? -eq 0 ]; then
	echo $prefix -- PASS
	numpassed=$((numpassed+1))
    else
	echo $prefix -- FAIL
    fi
done
echo $numpassed / $numtests tests passed.

#../bin/dsgrn "$(< test1.in)" > test1.txt
#diff -q test1.out test1.txt || echo "Test 1 failed!"
