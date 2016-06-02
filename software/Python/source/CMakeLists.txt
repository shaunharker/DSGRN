# add the executable

set ( LIBS      ${LIBS}
                ${Boost_LIBRARIES}
                ${PYTHON_LIBRARIES}
                sqlite3 
                dsgrn )

add_library ( libpydsgrnshared SHARED pyDSGRN.cpp )
set_target_properties( libpydsgrnshared PROPERTIES OUTPUT_NAME pyDSGRN SUFFIX .so )
target_link_libraries ( libpydsgrnshared ${LIBS} )

install(TARGETS libpydsgrnshared
        LIBRARY DESTINATION ${CMAKE_SOURCE_DIR}/modules/DSGRN)
