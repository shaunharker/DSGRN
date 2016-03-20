
set ( LIBS ${LIBS}
           ${Boost_LIBRARIES}
           libdsgrn
           libdsgrnpatternmatch
           sqlite3 )

set( TARGETS 
        testDataStructures
        testPatternMatch )

foreach ( TARGET ${TARGETS} ) 
  add_executable ( ${TARGET} ${TARGET}.cpp )
  target_link_libraries ( ${TARGET} ${LIBS} )
endforeach ( TARGET ) 
