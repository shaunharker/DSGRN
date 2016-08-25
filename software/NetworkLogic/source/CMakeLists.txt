# add the executable

set ( LIBS      ${LIBS}
                ${Boost_LIBRARIES} )

add_executable ( NetworkLogic NetworkLogic.cpp )
target_link_libraries ( NetworkLogic ${LIBS} )

install(TARGETS NetworkLogic 
        RUNTIME DESTINATION ${CMAKE_SOURCE_DIR}/bin )

add_executable ( SampleLogic SampleLogic.cpp )
target_link_libraries ( SampleLogic ${LIBS} )

install(TARGETS SampleLogic 
        RUNTIME DESTINATION ${CMAKE_SOURCE_DIR}/bin )
