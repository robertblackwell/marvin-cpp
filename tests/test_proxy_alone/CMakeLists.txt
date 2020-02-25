file(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
set(TARGET test_proxy_simple)
add_executable(${TARGET} 
	tp_main_simple.cpp
)
target_include_directories(${TARGET} PUBLIC ../test_helpers ../test_server_client ${TEST_INCLUDE_DIRS})
target_compile_features(${TARGET} PRIVATE cxx_std_17)
target_link_libraries(${TARGET} PUBLIC marvin_library ${TEST_LINK_LIBS})

# test_proxy_simple is NOT part of the ctest suite as it needs manual intervention to run
# see comments in tp_main_simple.cpp

#add_test(${TARGET} ${TARGET})
#SET_TESTS_PROPERTIES(${TARGET}
#    PROPERTIES ENVIRONMENT "SSL_CERT_FILE=/usr/local/etc/openssl@1.1/cert.pem")


set(TARGET2 test_proxy_alone)
add_executable(${TARGET2} 
	tp_main.cpp
	tp_whiteacorn_test.cpp
	proxy_fixture.cpp
	../test_proxy/tp_testcase.cpp
	../test_helpers/test_headers.cpp
	# ..//test_proxy/tp_tunnel.cpp
	# ../test_helpers/server_runner.cpp
	# ../test_helpers/tp_proxy_runner.cpp
	# ../test_server_client/tsc_req_handler.cpp
	# ../test_helpers/tp_proxy_runner.cpp

)
target_include_directories(${TARGET2} PUBLIC ../test_proxy ../test_helpers ../test_server_client ${TEST_INCLUDE_DIRS})
target_compile_features(${TARGET2} PRIVATE cxx_std_17)
target_link_libraries(${TARGET2} PUBLIC marvin_library ${TEST_LINK_LIBS})

add_test(${TARGET2} ${TARGET2})
SET_TESTS_PROPERTIES(${TARGET2}
    PROPERTIES ENVIRONMENT "SSL_CERT_FILE=/usr/local/etc/openssl@1.1/cert.pem")

