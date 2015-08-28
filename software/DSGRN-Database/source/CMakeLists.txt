# DSGRN-Database.cpp CMake file

set ( LIBS ${LIBS} ${Boost_LIBRARIES} ${MPI_LIBRARIES} dsgrn sqlite3 )

add_executable ( DSGRN-Database DSGRN-Database.cpp )
target_link_libraries ( DSGRN-Database ${LIBS} )

if(MPI_COMPILE_FLAGS)
  set_target_properties(DSGRN-Database PROPERTIES
    COMPILE_FLAGS "${MPI_COMPILE_FLAGS}")
endif()

if(MPI_LINK_FLAGS)
  set_target_properties(DSGRN-Database PROPERTIES
    LINK_FLAGS "${MPI_LINK_FLAGS}")
endif()

install(TARGETS DSGRN-Database 
        RUNTIME DESTINATION ${CMAKE_SOURCE_DIR}/bin )
