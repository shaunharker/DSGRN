#!/bin/bash

for file in `ls data/*.db`; do
    x=`sqlite3 $file "SELECT count(*) FROM sqlite_master WHERE type='table' AND name='MorseGraphZoo';"`
    if [ "$x" == "0" ]; then
      echo "Postprocessing " $file
      cat shell/postprocess.sql | sqlite3 $file > /dev/null
    fi
done
