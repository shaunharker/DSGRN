#!/bin/bash
absolute() { echo "$(cd "$(dirname "$1")" && pwd)/$(basename "$1")"; }
if [ $# -ge 1 ]; then
  PREFIX=$(absolute $1)
else
  PREFIX=/usr/local
fi
mkdir sqlite3Build && cd sqlite3Build
wget https://www.sqlite.org/snapshot/sqlite-amalgamation-201506261847.zip
unzip sqlite-amalgamation-201506261847
gcc shell.c sqlite3.c -lpthread -ldl -o sqlite3
gcc -shared -fPIC -o libsqlite3.so sqlite3.c -lpthread -ldl
mkdir -p $PREFIX/include && mv *.h $PREFIX/include
mkdir -p $PREFIX/bin && mv sqlite3 $PREFIX/bin
mkdir -p $PREFIX/lib && mv *.so $PREFIX/lib
cd .. && rm -rf sqlite3Build
