set(TEST_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(MARVIN_CERT_STORE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/fixtures/CA)
set(TEST_INCLUDE_DIRS 
	${TEST_DIR} 
	${HEADER_DIRS} 
	SYSTEM ${VENDOR_DIR}/include)

set(TEST_LINK_LIBS 
	marvin_library 
	Threads::Threads 
	${CMAKE_DL_LIBS}
	${VENDOR_DIR}/lib/libcrypto.a 
	${VENDOR_DIR}/lib/libssl.a 
	${VENDOR_DIR}/lib/libboost_filesystem.a 
	${VENDOR_DIR}/lib/libboost_system.a
	${VENDOR_DIR}/lib/libcert_library.a
	${CMAKE_DL_LIBS}
	${DL_LIB_SO}
	)
set(TEST_SERVER_BASEURL "http://whiteacorn.com/utests/echo/test.php")
add_subdirectory(test_doctest)
add_subdirectory(test_buffers)
add_subdirectory(test_timeout)
add_subdirectory(test_marvin_errors)
add_subdirectory(test_headers)
add_subdirectory(test_message)
add_subdirectory(test_server_v1)
add_subdirectory(test_server_v2)
# add_subdirectory(test_message_factory)
# add_subdirectory(test_message_parser)
add_subdirectory(test_message_reader)
add_subdirectory(test_message_roundtrip)

add_subdirectory(test_uri)

add_subdirectory(test_connect)
add_subdirectory(test_ssl)
# need to come back to this when socketFactory knows how to make ssl connections
# add_subdirectory(test_connect)
add_subdirectory(test_request)
add_subdirectory(test_client_request)
add_subdirectory(test_server_client_bb)
add_subdirectory(test_server_client_bf)
add_subdirectory(test_proxy)
add_subdirectory(test_proxy_manual)

add_subdirectory(test_tcp)

# Note have to do the add_test and set_tests_properties in the sub_directories

add_subdirectory(test_server_setup_fixture)
# add_test(server_setup server_setup)
add_subdirectory(test_server_cleanup_fixture)
# add_test(server_cleanup server_cleanup)

# set_tests_properties(test_tcp PROPERTIES FIXTURES_REQUIRED Server)

# set_tests_properties(server_setup    PROPERTIES FIXTURES_SETUP    Server)
# set_tests_properties(server_cleanup  PROPERTIES FIXTURES_CLEANUP  Server)
