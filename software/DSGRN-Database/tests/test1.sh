#!/bin/bash
SRC_ROOT=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
rm -f $SRC_ROOT/test.db
$SRC_ROOT/../build/bin/DSGRN-Database $SRC_ROOT/../../../networks/2D_Example.txt $SRC_ROOT/test.db || exit 1
X=`sqlite3 $SRC_ROOT/test.db 'select count(*) from MorseGraphViz;'` || exit 1
if [[ "$X" -eq "19" ]]; then
  exit 0
else
  exit 1
fi
