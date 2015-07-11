
cmake_minimum_required (VERSION 2.6) 
project (DSGRN_NetworkLogic) 

#########
# rpath #
#########

set(CMAKE_MACOSX_RPATH 1)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

#########
# C++11 #
#########

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -ftemplate-depth=2048")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -fprofile-arcs -ftest-coverage")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -DNDEBUG" )

############
# Packages #
############

find_package(Boost 1.58.0 REQUIRED COMPONENTS serialization)

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

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

###########
# Recurse #
###########

add_subdirectory (source)

