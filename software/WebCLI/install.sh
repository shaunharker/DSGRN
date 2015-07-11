#!/bin/bash
absolute() { echo "$(cd "$(dirname "$1")" && pwd)/$(basename "$1")"; }

# Prepare web assets: Logic files and Networks
SRC_DIR=$(absolute ../../.. )
echo $SRC_DIR
cd $SRC_DIR/software/NetworkLogic
LOGIC_DIR=$SRC_DIR/software/WebCLI/asset_dir/share/DSGRN/logic
mkdir -p $LOGIC_DIR       || exit 1
./bin/NetworkLogic 4      || exit 1
mv -f *.dat $LOGIC_DIR    || exit 1
cp -rf $SRC_DIR/networks $SRC_DIR/software/WebCLI/asset_dir/
cd $SRC_DIR/software/WebCLI
echo `pwd`

# Prepare directories
rm -rf lib
rm -rf include
rm -f dsgrn*
mkdir -p lib
mkdir -p include

# libboost_serialization.bc
echo `pwd`
cd libs/boost
./emscripten-install.sh
cp ./lib/*.bc ../../lib
cd ../..

# libsqlite3.bc
cd libs/sqlite3
./emscripten-install.sh
cp ./lib/*.bc ../../lib
cp ./include/*.h ../../include
cd ../..

# libdsgrn.bc
emcc -std=c++11 -O3 $SRC_DIR/source/library.cpp -I$SRC_DIR/include -I/usr/local/include -I./include -DINSTALLPREFIX='"asset_dir"' ./lib/lib* -o ./lib/libdsgrn.bc

# dsgrn.js
emcc -std=c++11 -O3 $SRC_DIR/source/DSGRN.cpp -I$SRC_DIR/include -I/usr/local/include -I./include -DINSTALLPREFIX='"asset_dir"' ./lib/libdsgrn.bc  -o dsgrn.js -s DISABLE_EXCEPTION_CATCHING=0 --embed-file ../asset_dir

mv dsgrn.js middle.js
echo "function DSGRNPROGRAM (Module) {" > dsgrn.js
cat middle.js >> dsgrn.js
echo "" >> dsgrn.js
echo "return Module; }" >> dsgrn.js
rm middle.js


