#!/bin/bash
# install.sh [--prefix=PREFIX] [--build=BUILDTYPE]    \
#            [--search=SEARCHPATH] [CMake arguments]
#  
#  Build the project with the supplied configurations,
#    or else default values.
#
#   PREFIX gives the location to install.
#   BUILDTYPE is either Debug or Release 
#     (or some other CMake recognizable build type)
#   SEARCHPATH is an optional location to search for headers 
#     and libraries (i.e. SEARCHPATH/include and SEARCHPATH/lib)
#   The default setting for PREFIX is /usr/local unless it is not writable
#     in which case it is ~/.local.
#   The default setting for BUILDTYPE is Release
#   The default setting for SEARCHPATH is to be equal to PREFIX
#   Additional arguments will be passed to CMake. Any paths in these arguments
#   should be absolute.

SRC_ROOT=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
build="$SRC_ROOT/../../.install/build.sh"

# Parse command line arguments
source $SRC_ROOT/../../.install/parse.sh

# Build "pyDSGRN"
cd ${SRC_ROOT}
$build --prefix=$PREFIX --searchpath=$SEARCHPATH --build=$BUILDTYPE $MASS || exit 1

# Install pyDSGRN
mkdir -p $PREFIX/share/DSGRN/modules         || exit 1
cp -rf modules/DSGRN $PREFIX/share/DSGRN/modules || exit 1

# Give information about how to integrate with python
echo "===================================="
echo "$(tput setaf 5)Important information$(tput sgr0)"
echo "The python module \"DSGRN\" has been built and saved in "
echo "  $(tput setaf 1)$PREFIX/share/DSGRN/modules/DSGRN$(tput sgr0)"
echo "To be able to use the module with python, you must either "
echo "(1) Copy the module into your python distribution by typing "
echo "  $(tput setaf 6)cd modules$(tput sgr0)"
echo "  $(tput setaf 6)python setup.py install$(tput sgr0)"
echo "or else"
echo "(2) Add the line  "
echo "  $(tput setaf 6)export PYTHONPATH=$PREFIX/share/DSGRN/modules:""$""PYTHONPATH$(tput sgr0) "
echo "to your $(tput setaf 1)~/.bashrc$(tput sgr0) file. See the $(tput setaf 1)README.md$(tput sgr0) file for more details."
echo "===================================="

