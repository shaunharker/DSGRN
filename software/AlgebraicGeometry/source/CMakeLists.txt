# ParameterSampler.cpp CMake file

set ( LIBS ${LIBS} ${Boost_LIBRARIES} dsgrn sqlite3 )

add_executable ( ParameterSampler ParameterSampler.cpp )
target_link_libraries ( ParameterSampler ${LIBS} )

install(TARGETS ParameterSampler 
        RUNTIME DESTINATION ${CMAKE_SOURCE_DIR}/bin )
