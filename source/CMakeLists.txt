# add the executable

set ( LIBS      ${LIBS}
                boost_system
                boost_thread
                boost_serialization 
                sqlite3 )

add_library ( libdsgrn STATIC library.cpp )
set_target_properties( libdsgrn PROPERTIES OUTPUT_NAME dsgrn )

add_library ( libdsgrnshared SHARED library.cpp )
set_target_properties( libdsgrnshared PROPERTIES OUTPUT_NAME dsgrn )
target_link_libraries ( libdsgrnshared ${LIBS} )

add_executable ( dsgrn dsgrn.cpp )
target_link_libraries ( dsgrn ${LIBS} libdsgrn )

install(TARGETS dsgrn libdsgrn libdsgrnshared
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)
