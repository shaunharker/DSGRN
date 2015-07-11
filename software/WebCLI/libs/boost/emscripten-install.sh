#!/bin/bash

em++ -O3 -std=c++11 -o libboost_serialization.bc ./source/*.cpp -I./include -I/usr/local/include
mkdir -p lib
mv *bc lib

