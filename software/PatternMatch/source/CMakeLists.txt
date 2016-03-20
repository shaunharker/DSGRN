# add the executable

set ( LIBS      ${LIBS}
                ${Boost_LIBRARIES}
                sqlite3 )

set ( LIBRARYSOURCEFILES SearchGraph.cpp PatternGraph.cpp MatchingGraph.cpp PatternMatch.cpp)
add_library ( libpatternmatch STATIC ${LIBRARYSOURCEFILES} )
set_target_properties( libpatternmatch PROPERTIES OUTPUT_NAME dsgrnpatternmatch )

add_library ( libpatternmatchshared SHARED ${LIBRARYSOURCEFILES} )
set_target_properties( libpatternmatchshared PROPERTIES OUTPUT_NAME dsgrnpatternmatch )
target_link_libraries ( libpatternmatchshared ${LIBS} )

#add_executable ( ExecutableName ExecutableName.cpp)
#target_link_libraries ( ExecutableName libdsgrnpatternmatch ${LIBS} )

install(TARGETS libpatternmatch libpatternmatchshared
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)
