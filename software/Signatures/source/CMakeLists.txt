# Signatures.cpp CMake file

set ( LIBS ${LIBS} ${Boost_LIBRARIES} ${MPI_LIBRARIES} dsgrn sqlite3 )

add_executable ( Signatures Signatures.cpp )
target_link_libraries ( Signatures ${LIBS} )

if(MPI_COMPILE_FLAGS)
  set_target_properties(Signatures PROPERTIES
    COMPILE_FLAGS "${MPI_COMPILE_FLAGS}")
endif()

if(MPI_LINK_FLAGS)
  set_target_properties(Signatures PROPERTIES
    LINK_FLAGS "${MPI_LINK_FLAGS}")
endif()

install(TARGETS Signatures 
        RUNTIME DESTINATION ${CMAKE_SOURCE_DIR}/bin )
