file(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)


set(TARGET2 test_proxy)
add_executable(${TARGET2} 
	tp_main.cpp
	tp_whiteacorn_test.cpp
	proxy_fixture.cpp
	tp_testcase.cpp
	../test_helpers/test_headers.cpp

)
target_include_directories(${TARGET2} PUBLIC ../test_proxy ../test_helpers ../test_server_client ${TEST_INCLUDE_DIRS})
target_compile_features(${TARGET2} PRIVATE cxx_std_17)
target_link_libraries(${TARGET2} PUBLIC marvin_library ${TEST_LINK_LIBS})

add_test(${TARGET2} ${TARGET2})
SET_TESTS_PROPERTIES(${TARGET2}
    PROPERTIES ENVIRONMENT "SSL_CERT_FILE=/usr/local/etc/openssl@1.1/cert.pem")

