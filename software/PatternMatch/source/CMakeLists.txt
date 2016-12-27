# add the executable

set ( LIBS      ${LIBS}
                ${Boost_LIBRARIES}
                dsgrn
                sqlite3 
                ${CMAKE_THREAD_LIBS_INIT} )

add_executable ( PatternMatchDatabase PatternMatchDatabase.cpp)
target_link_libraries ( PatternMatchDatabase ${LIBS} ${MPI_LIBRARIES} )

add_executable ( NumericalSimulationPatternMatch NumericalSimulationPatternMatch.cpp)
target_link_libraries ( NumericalSimulationPatternMatch ${LIBS} ${MPI_LIBRARIES} )

if(MPI_COMPILE_FLAGS)
  set_target_properties(PatternMatchDatabase PROPERTIES
    COMPILE_FLAGS "${MPI_COMPILE_FLAGS}")
  set_target_properties(NumericalSimulationPatternMatch PROPERTIES
    COMPILE_FLAGS "${MPI_COMPILE_FLAGS}")
endif()

if(MPI_LINK_FLAGS)
  set_target_properties(PatternMatchDatabase PROPERTIES
    LINK_FLAGS "${MPI_LINK_FLAGS}")
  set_target_properties(NumericalSimulationPatternMatch PROPERTIES
    LINK_FLAGS "${MPI_LINK_FLAGS}")
endif()

install(TARGETS NumericalSimulationPatternMatch PatternMatchDatabase
        RUNTIME DESTINATION ${CMAKE_SOURCE_DIR}/bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)
