#!/bin/bash
mkdir temp
cd temp
wget https://www.sqlite.org/snapshot/sqlite-amalgamation-201506261847.zip
unzip sqlite-amalgamation-201506261847
emcc -fPIC -o libsqlite3.bc sqlite3.c
mv *.bc ../
mv *.h ../
cd ../
rm -rf temp
mkdir -p include
mkdir -p lib
mv *.h include
mv *.bc lib
