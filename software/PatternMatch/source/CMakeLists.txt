# add the executable

set ( LIBS      ${LIBS}
                ${Boost_LIBRARIES}
                dsgrn
                sqlite3 )

set ( LIBRARYSOURCEFILES Pattern.cpp SearchGraph.cpp PatternGraph.cpp MatchingGraph.cpp PatternMatch.cpp)
add_library ( libdsgrnpatternmatch STATIC ${LIBRARYSOURCEFILES} )
set_target_properties( libdsgrnpatternmatch PROPERTIES OUTPUT_NAME dsgrnpatternmatch )

add_library ( libdsgrnpatternmatchshared SHARED ${LIBRARYSOURCEFILES} )
set_target_properties( libdsgrnpatternmatchshared PROPERTIES OUTPUT_NAME dsgrnpatternmatch )
target_link_libraries ( libdsgrnpatternmatchshared ${LIBS} )

add_executable ( PatternMatchDatabase PatternMatchDatabase.cpp)
target_link_libraries ( PatternMatchDatabase libdsgrnpatternmatch ${LIBS} ${MPI_LIBRARIES} )

add_executable ( NumericalSimulationPatternMatch NumericalSimulationPatternMatch.cpp)
target_link_libraries ( NumericalSimulationPatternMatch libdsgrnpatternmatch ${LIBS} ${MPI_LIBRARIES} )

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

install(TARGETS NumericalSimulationPatternMatch PatternMatchDatabase libdsgrnpatternmatch libdsgrnpatternmatchshared
        RUNTIME DESTINATION ${CMAKE_SOURCE_DIR}/bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)
