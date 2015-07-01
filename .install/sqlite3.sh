#!/bin/bash

## Parse command line arguments to get install PREFIX
SHELL_DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
source $SHELL_DIR/parse.sh

## Download
mytmpdir=`mktemp -d 2>/dev/null || mktemp -d -t 'mytmpdir'` && cd $mytmpdir
wget https://www.sqlite.org/snapshot/sqlite-amalgamation-201506261847.zip
unzip sqlite-amalgamation-201506261847

## Compile
gcc shell.c sqlite3.c -lpthread -ldl -o sqlite3
gcc -shared -fPIC -o libsqlite3.so sqlite3.c -lpthread -ldl

## Install
mkdir -p $PREFIX/include && mv *.h $PREFIX/include
mkdir -p $PREFIX/bin && mv sqlite3 $PREFIX/bin
mkdir -p $PREFIX/lib && mv *.so $PREFIX/lib

## Clean up
rm -rf $mytmpdir
