cmake_minimum_required(VERSION 2.8.12)
project(DSGRN)

add_subdirectory(pybind11)

include_directories ( 
  ${CMAKE_SOURCE_DIR}/include
  /usr/local/include 
  ${USER_INCLUDE_PATH} )

link_directories ( 
  ${USER_LIBRARY_PATH} 
  /usr/local/lib )

message("USER INCLUDE PATH IS ${USER_INCLUDE_PATH}")

pybind11_add_module(_dsgrn src/DSGRN/_dsgrn/DSGRN.cpp)
