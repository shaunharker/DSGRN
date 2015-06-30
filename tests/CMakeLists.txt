# add the executable

set ( LIBS      ${LIBS}
                ${Boost_LIBRARIES}
                libdsgrn
                sqlite3 )

add_executable ( TestAnnotation.x TestAnnotation.cpp )
target_link_libraries ( TestAnnotation.x ${LIBS} )

add_executable ( TestDomain.x TestDomain.cpp )
target_link_libraries ( TestDomain.x ${LIBS} )

add_executable ( TestWall.x TestWall.cpp )
target_link_libraries ( TestWall.x ${LIBS} )

add_executable ( TestDomainGraph.x TestDomainGraph.cpp )
target_link_libraries ( TestDomainGraph.x ${LIBS} )

add_executable ( TestWallGraph.x TestWallGraph.cpp )
target_link_libraries ( TestWallGraph.x ${LIBS} )

add_executable ( TestMorseDecomposition.x TestMorseDecomposition.cpp )
target_link_libraries ( TestMorseDecomposition.x ${LIBS} )

add_executable ( TestMorseGraph.x TestMorseGraph.cpp )
target_link_libraries ( TestMorseGraph.x ${LIBS} )

add_executable ( TestNetwork.x TestNetwork.cpp )
target_link_libraries ( TestNetwork.x ${LIBS} )

add_executable ( TestOrderParameter.x TestOrderParameter.cpp )
target_link_libraries ( TestOrderParameter.x ${LIBS} )

add_executable ( TestParameterGraph.x TestParameterGraph.cpp )
target_link_libraries ( TestParameterGraph.x ${LIBS} )


install(TARGETS 
        TestAnnotation.x
        TestDomain.x
        TestWall.x        
        TestDomainGraph.x
        TestWallGraph.x
        TestMorseDecomposition.x
        TestMorseGraph.x
        TestNetwork.x
        TestOrderParameter.x
        TestParameterGraph.x
        RUNTIME DESTINATION ${CMAKE_SOURCE_DIR}/tests )
