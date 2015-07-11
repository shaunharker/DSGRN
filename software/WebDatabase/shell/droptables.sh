#!/bin/bash

for file in `ls data/*.db`; do
    sqlite3 $file "drop table MorseGraphZoo;"
done
