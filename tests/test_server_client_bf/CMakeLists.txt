
set(TARGET_2 test_server_client_bf)
add_executable(${TARGET_2} 
	bf_main.cpp
	bf_tests.cpp
	bf_post.cpp
	bf_testcase.cpp
	bf_pipeline.cpp
	../test_server_client/tsc_req_handler.cpp
	../test_helpers/server_runner.cpp
)
target_include_directories(${TARGET_2} PUBLIC ./ ./bf ../test_helpers ../test_server_client ${TEST_INCLUDE_DIRS})
target_compile_features(${TARGET_2} PRIVATE cxx_std_14)
target_link_libraries(${TARGET_2} PUBLIC marvin_library ${TEST_LINK_LIBS})
target_precompile_headers(${TARGET_2} REUSE_FROM marvin_library)

add_test(${TARGET_2} ${TARGET_2})
SET_TESTS_PROPERTIES(${TARGET_2}
    PROPERTIES ENVIRONMENT "SSL_CERT_FILE=/usr/local/etc/openssl@1.1/cert.pem")

