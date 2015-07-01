#!/bin/bash

CUR_DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
#Parse argument
if [ $# -ge 1 ]; then
    PREFIX=${1}
    mkdir -p "${PREFIX}" 2> /dev/null
    cd "${PREFIX}" > /dev/null 2>&1
    if [ $? != 0 ] ; then
        echo "ERROR: directory '${PREFIX}' does not exist nor could be created."
        echo "Please choose another directory."
        exit 1
    else
        PREFIX=`pwd -P`
    fi
    echo "DSGRN will be installed in '${PREFIX}'"    
    ARGUMENT="-DMYPREFIX=${PREFIX} -DCMAKE_INSTALL_PREFIX=${PREFIX} -DUSER_INCLUDE_PATH=${PREFIX}/include -DUSER_LIBRARY_PATH=${PREFIX}/lib"

else
    ARGUMENT="-DMYPREFIX=/usr/local"
fi

if [ $# -ge 2 ]; then
    ARGUMENT="$ARGUMENT -DCMAKE_BUILD_TYPE=${2}"
else
    ARGUMENT="$ARGUMENT -DCMAKE_BUILD_TYPE=Release"
fi

cd ${CUR_DIR}
# Build NetworkLogic
./software/NetworkLogic/install.sh $PREFIX
mkdir -p $PREFIX/share/DSGRN/logic
./software/NetworkLogic/bin/NetworkLogic 4
mv -f *.dat $PREFIX/share/DSGRN/logic/
# Build DSGRN
rm -rf build
mkdir build
cd build
cmake $ARGUMENT ..
make || exit 1
make install || exit 1
make test
# Check if tests failed:
if [ ! $? -eq 0 ]; then
  cat build/Testing/Temporary/LastTest.log
  exit 1
else
  exit 0
fi

