file(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
set(TARGET test_message)
add_executable(${TARGET} 
	test_message.cpp
	test_message_main.cpp
	../test_server_client/tsc_req_handler.cpp
	../test_helpers/server_runner.cpp
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