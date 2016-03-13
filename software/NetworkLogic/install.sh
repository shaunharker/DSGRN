#SRC_ROOT=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
SRC_ROOT=../..
build="$SRC_ROOT/.install/build.sh"

# Parse command line arguments
source $SRC_ROOT/.install/parse.sh

# Build "NetworkLogic"
cd $SRC_ROOT/software/NetworkLogic
$build --prefix=$PREFIX --searchpath=$SEARCHPATH --build=$BUILDTYPE $MASS || exit 1
