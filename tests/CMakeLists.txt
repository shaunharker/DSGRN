
set ( LIBS ${LIBS}
           ${Boost_LIBRARIES}
           libdsgrn
           sqlite3 )

set( TARGETS 
        TestAnnotation
        TestComponents
        TestStrongComponents
        TestDigraph
        TestDomain
        TestWall        
        TestDomainGraph
        TestWallGraph
        TestMorseDecomposition
        TestMorseGraph
        TestNetwork
        TestOrderParameter
        TestParameter
        TestParameterGraph
      	TestPoset )
        
foreach ( TARGET ${TARGETS} ) 
  add_executable ( ${TARGET} ${TARGET}.cpp )
  target_link_libraries ( ${TARGET} ${LIBS} )
endforeach ( TARGET ) 

#install(TARGETS ${TARGETS} 
#        RUNTIME DESTINATION ${CMAKE_SOURCE_DIR}/tests )
