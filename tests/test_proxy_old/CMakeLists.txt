file(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
set(TARGET test_proxy)
add_executable(${TARGET} 
	tp_main.cpp
	tp_testcase.cpp
	tp_proxy_tests.cpp
	tp_post.cpp
	tp_tunnel.cpp
	../test_helpers/server_runner.cpp
	../test_helpers/tp_proxy_runner.cpp
	../test_server_client/tsc_req_handler.cpp
	../test_helpers/test_headers.cpp
	../test_helpers/tp_proxy_runner.cpp

)
target_include_directories(${TARGET} PUBLIC ../test_helpers ../test_server_client ${TEST_INCLUDE_DIRS})
target_link_libraries(${TARGET} PUBLIC marvin_library ${TEST_LINK_LIBS})

add_test(${TARGET} ${TARGET})
add_test(${TARGET} ${TARGET})
SET_TESTS_PROPERTIES(${TARGET}
    PROPERTIES ENVIRONMENT "SSL_CERT_FILE=/usr/local/etc/openssl@1.1/cert.pem")


# set_tests_properties(${TARGET}
# 	PROPERTIES
# 	PASS_REGULAR_EXPRESSION "Test passed")
# set_tests_properties(${TARGET}
# 	PROPERTIES
# 	FAIL_REGULAR_EXPRESSION "(Exception|Test failed)")
# set_tests_properties(${TARGET}
# 	PROPERTIES
# 	TIMEOUT 120)