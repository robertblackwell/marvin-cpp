set(TEST_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(MARVIN_CERT_STORE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/fixtures/CA)
set(TEST_INCLUDE_DIRS 
	${TEST_DIR} 
	${HEADER_DIRS} 
	SYSTEM ${VENDOR_DIR}/include)

set(TEST_LINK_LIBS marvin_library Threads::Threads ${CMAKE_DL_LIBS}
	${VENDOR_DIR}/lib/libcrypto.a 
	${VENDOR_DIR}/lib/libssl.a 
	${VENDOR_DIR}/lib/libboost_filesystem.a 
	${VENDOR_DIR}/lib/libboost_system.a
	${VENDOR_DIR}/lib/libcert_library.a
	)

add_subdirectory(test_message)
add_subdirectory(test_headers)
add_subdirectory(test_connect)
add_subdirectory(test_ssl)
add_subdirectory(test_message_parser)
add_subdirectory(test_buffers)
# need to come back to this when socketFactory knows how to make ssl connections
# add_subdirectory(test_connect)
add_subdirectory(test_client_request)
add_subdirectory(test_timeout)
add_subdirectory(reader_test)
add_subdirectory(test_server_client)
add_subdirectory(test_proxy)
add_subdirectory(test_proxy_manual)
