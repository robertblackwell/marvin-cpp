file(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
set(TARGET test_server_client_bb)
add_executable(${TARGET} 
	bb_main.cpp
	bb/bb_tests.cpp
	bb/bb_client.cpp
	tsc_req_handler.cpp
	bb/bb_testcase_defs.cpp
	bb/bb_testcase.cpp
	../test_helpers/server_runner.cpp
)
target_include_directories(${TARGET} PUBLIC ./ ./bb ../reader_test ../test_helpers ../test_server_client ${TEST_INCLUDE_DIRS})
target_compile_features(${TARGET} PRIVATE cxx_std_14)
target_link_libraries(${TARGET} PUBLIC marvin_library ${TEST_LINK_LIBS})

add_test(${TARGET} ${TARGET})
SET_TESTS_PROPERTIES(${TARGET}
    PROPERTIES ENVIRONMENT "SSL_CERT_FILE=/usr/local/etc/openssl@1.1/cert.pem")

set(TARGET_2 test_server_client_bf)
add_executable(${TARGET_2} 
	bf_main.cpp
	bftests.cpp
	bf/bf_post.cpp
	../test_helpers/server_runner.cpp
	bf/bf_testcase.cpp
	tsc_req_handler.cpp
	bf/bf_pipeline.cpp
)
target_include_directories(${TARGET_2} PUBLIC ./ ./bf ../test_helpers ../test_server_client ${TEST_INCLUDE_DIRS})
target_compile_features(${TARGET_2} PRIVATE cxx_std_14)
target_link_libraries(${TARGET_2} PUBLIC marvin_library ${TEST_LINK_LIBS})

add_test(${TARGET_2} ${TARGET_2})
SET_TESTS_PROPERTIES(${TARGET_2}
    PROPERTIES ENVIRONMENT "SSL_CERT_FILE=/usr/local/etc/openssl@1.1/cert.pem")

