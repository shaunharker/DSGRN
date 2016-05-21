# add the executable

set ( LIBS      ${LIBS}
                ${Boost_LIBRARIES}
                ${PYTHON_LIBRARIES}
                sqlite3 )

add_library ( libpydsgrnshared SHARED pyDSGRN.cpp )
set_target_properties( libpydsgrnshared PROPERTIES OUTPUT_NAME pyDSGRN SUFFIX .so)
target_link_libraries ( libpydsgrnshared ${LIBS} )

#install(TARGETS libpydsgrnshared
#        RUNTIME DESTINATION bin
#        LIBRARY DESTINATION lib
#        ARCHIVE DESTINATION lib)
