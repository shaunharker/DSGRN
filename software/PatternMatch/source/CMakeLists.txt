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

#add_executable ( ExecutableName ExecutableName.cpp)
#target_link_libraries ( ExecutableName libdsgrnpatternmatch ${LIBS} )

install(TARGETS libdsgrnpatternmatch libdsgrnpatternmatchshared
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)
