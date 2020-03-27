file(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
set(TARGET test_message_reader)
add_executable(${TARGET} 
	reader_main.cpp
	mock_socket.cpp
	repeating_timer.cpp
	testcase.cpp
	test_runner.cpp
	testcase_defs.cpp
	t_client.cpp
	t_server.cpp
)
target_include_directories(${TARGET} PUBLIC ./ ../test_helpers ../test_server_client ${TEST_INCLUDE_DIRS})
target_precompile_headers(${TARGET} REUSE_FROM marvin_library)
target_link_libraries(${TARGET} PUBLIC marvin_library ${TEST_LINK_LIBS})

add_test(${TARGET} ${TARGET})
add_test(${TARGET} ${TARGET})
SET_TESTS_PROPERTIES(${TARGET}
    PROPERTIES ENVIRONMENT "SSL_CERT_FILE=/usr/local/etc/openssl@1.1/cert.pem")

# set(TARGET_2 test_reader)
# add_executable(${TARGET_2} 
# 	reader_main.cpp
# 	testcase.cpp
# 	test_runner.cpp
# 	testcase_defs.cpp
# 	t_client.cpp
# 	t_server.cpp
# )
# target_include_directories(${TARGET_2} PUBLIC ./ ../test_helpers ../test_server_client ${TEST_INCLUDE_DIRS})
# target_compile_features(${TARGET_2} PRIVATE cxx_std_14)
# target_link_libraries(${TARGET_2} PUBLIC marvin_library ${TEST_LINK_LIBS})

# add_test(${TARGET_2} ${TARGET_2})
# add_test(${TARGET_2} ${TARGET_2})
# SET_TESTS_PROPERTIES(${TARGET_2}
#     PROPERTIES ENVIRONMENT "SSL_CERT_FILE=/usr/local/etc/openssl@1.1/cert.pem")

