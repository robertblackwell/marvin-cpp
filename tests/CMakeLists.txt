set(TEST_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(TEST_INCLUDE_DIRS 
	${TEST_DIR} 
	${HEADER_DIRS} 
	SYSTEM ${VENDOR_DIR}/include)
set(TEST_LINK_LIBS marvin_library 
	${VENDOR_DIR}/lib/libcrypto.a 
	${VENDOR_DIR}/lib/libssl.a 
	${VENDOR_DIR}/lib/libboost_filesystem.a 
	${VENDOR_DIR}/lib/libboost_system.a
	)

add_subdirectory(test_message)
add_subdirectory(test_headers)
add_subdirectory(test_message_parser)
add_subdirectory(test_buffers)
add_subdirectory(test_connect)
add_subdirectory(test_client_request)
add_subdirectory(test_timeout)
add_subdirectory(reader_test)
add_subdirectory(test_server_client)
add_subdirectory(test_proxy)
add_subdirectory(test_proxy_manual)
