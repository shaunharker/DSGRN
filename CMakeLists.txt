
cmake_minimum_required (VERSION 2.6) 
project (DSGRN) 

#########
# rpath #
#########

set(CMAKE_MACOSX_RPATH 1)
set(CMAKE_SKIP_BUILD_RPATH  FALSE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE) 
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
list(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/lib" isSystemDir)
if("${isSystemDir}" STREQUAL "-1")
   set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
endif("${isSystemDir}" STREQUAL "-1")

#########
# C++11 #
#########

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O3 -DNDEBUG")

###############
# Definitions #
###############

add_definitions( -DINSTALLPREFIX="${MYPREFIX}" )
message( STATUS "Chose the following prefix: " ${MYPREFIX} )

############
# Packages #
############

find_package(Boost 1.58.0 REQUIRED COMPONENTS system thread serialization)

#######################
# Include Directories #
#######################

include_directories ( 
  ./include 
  /usr/local/include 
  /opt/local/include 
  ${Boost_INCLUDE_DIRS} 
  ${USER_INCLUDE_PATH} )

#######################
# Library Directories #
#######################

link_directories ( 
  ${USER_LIBRARY_PATH} 
  /usr/local/lib 
  /opt/local/lib )

###########
# Outputs #
###########

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

###########
# Recurse #
###########

add_subdirectory (source)

#########
# Tests #
#########

enable_testing()
add_test( test1 ../tests/test.sh test1 )
add_test( test2 ../tests/test.sh test2 )
add_test( test3 ../tests/test.sh test3 )
add_test( test4 ../tests/test.sh test4 )
