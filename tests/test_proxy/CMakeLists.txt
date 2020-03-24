file(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)


set(TARGET test_proxy)
add_executable(${TARGET} 
	tp_main.cpp
	tp_whiteacorn_test.cpp
	proxy_fixture.cpp
	tp_testcase.cpp
	../test_helpers/test_headers.cpp

)
target_include_directories(${TARGET} PUBLIC ../test_proxy ../test_helpers ../test_server_client ${TEST_INCLUDE_DIRS})
target_precompile_headers(${TARGET} REUSE_FROM marvin_library)
target_link_libraries(${TARGET} PUBLIC marvin_library ${TEST_LINK_LIBS})

add_test(${TARGET} ${TARGET})
SET_TESTS_PROPERTIES(${TARGET}
    PROPERTIES ENVIRONMENT "SSL_CERT_FILE=/usr/local/etc/openssl@1.1/cert.pem")

