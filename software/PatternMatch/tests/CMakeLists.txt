
set ( LIBS ${LIBS}
           ${Boost_LIBRARIES}
           dsgrn
           libdsgrnpatternmatch
           sqlite3 )

set( TARGETS 
        TestPattern
        TestPatternGraph
        TestSearchGraph
        TestMatchingGraph 
        TestPatternMatch )

foreach ( TARGET ${TARGETS} ) 
  add_executable ( ${TARGET} ${TARGET}.cpp )
  target_link_libraries ( ${TARGET} ${LIBS} )
endforeach ( TARGET ) 
