
cmake_minimum_required (VERSION 2.6) 
project (DSGRN_Signatures) 

#########
# rpath #
#########

set(CMAKE_MACOSX_RPATH 1)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

#########
# C++11 #
#########

if( CMAKE_COMPILER_IS_GNUCXX )
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O3 -DNDEBUG -ftemplate-depth=2048")
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -stdlib=libc++ -O3 -DNDEBUG -ftemplate-depth=2048")
endif()

###############
# Definitions #
###############

add_definitions( -DINSTALLPREFIX="${MYPREFIX}" )
message( STATUS "Chose the following prefix: " ${MYPREFIX} )

############
# Packages #
############

find_package(Boost 1.58.0 REQUIRED COMPONENTS system thread serialization)
find_package(MPI REQUIRED)

#######################
# Include Directories #
#######################

include_directories ( 
  ./include 
  ../../include
  /usr/local/include 
  /opt/local/include 
  ${Boost_INCLUDE_DIRS} 
  ${MPI_INCLUDE_PATH}
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
