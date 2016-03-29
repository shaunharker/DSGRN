
cmake_minimum_required (VERSION 2.6) 
project (DSGRN-PatternMatch) 

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
# Flags #
#########

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -ftemplate-depth=2048")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -fprofile-arcs -ftest-coverage")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -DNDEBUG" )

###############
# Definitions #
###############

add_definitions( -DINSTALLPREFIX="${CMAKE_INSTALL_PREFIX}" )
message( STATUS "Chose the following prefix: " ${CMAKE_INSTALL_PREFIX} )

############
# Packages #
############

find_package(Boost 1.58.0 REQUIRED COMPONENTS serialization)
find_package(MPI REQUIRED)

#######################
# Include Directories #
#######################

include_directories ( 
  ${CMAKE_SOURCE_DIR}/include
  ${MPI_INCLUDE_PATH}
  ./include
  ../../include
  /usr/local/include 
  ${Boost_INCLUDE_DIRS} 
  ${USER_INCLUDE_PATH} )

#######################
# Library Directories #
#######################

link_directories ( 
  ${USER_LIBRARY_PATH} 
  /usr/local/lib )

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
add_subdirectory (tests)

#########
# Tests #
#########

enable_testing()
add_test ( TestPattern ${CMAKE_BINARY_DIR}/bin/TestPattern )
add_test ( TestPatternGraph ${CMAKE_BINARY_DIR}/bin/TestPatternGraph )
add_test ( TestSearchGraph ${CMAKE_BINARY_DIR}/bin/TestSearchGraph )
add_test ( TestMatchingGraph ${CMAKE_BINARY_DIR}/bin/TestMatchingGraph )
add_test ( TestPatternMatch ${CMAKE_BINARY_DIR}/bin/TestPatternMatch )
