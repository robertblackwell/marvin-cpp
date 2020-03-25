file(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
set(TARGET test_server_client_bb)
add_executable(${TARGET} 
	bb_main.cpp
	bb_tests.cpp
	bb_client.cpp
	bb_testcase_defs.cpp
	bb_testcase.cpp
	../test_server_client/tsc_req_handler.cpp
	../test_helpers/server_runner.cpp
)

target_include_directories(${TARGET} PUBLIC ./ ./bb ../reader_test ../test_helpers ../test_server_client ${TEST_INCLUDE_DIRS})
target_precompile_headers(${TARGET} REUSE_FROM marvin_library)
target_compile_features(${TARGET} PRIVATE cxx_std_14)
target_link_libraries(${TARGET} PUBLIC marvin_library ${TEST_LINK_LIBS})

add_test(${TARGET} ${TARGET})
SET_TESTS_PROPERTIES(${TARGET}
    PROPERTIES ENVIRONMENT "SSL_CERT_FILE=/usr/local/etc/openssl@1.1/cert.pem")
