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
add_subdirectory(test_buffers)
add_subdirectory(test_certificates)
add_subdirectory(test_headers)
add_subdirectory(test_marvin_errors)
add_subdirectory(test_message)
add_subdirectory(test_message_reader)
add_subdirectory(test_message_roundtrip)
add_subdirectory(test_proxy_manual)
add_subdirectory(test_run_shell) # demonstrates how to use a shell script as the test
# need to get these two working and decide which tests need these dependencies
# add_subdirectory(test_server_setup_fixture)
# add_subdirectory(test_server_cleanup_fixture)

add_subdirectory(test_server_v3)
# add_subdirectory(test_ssl)		# not sure this one will stay
add_subdirectory(test_timeout)
add_subdirectory(test_uri)
add_subdirectory(test_subcommands)

if (OFF)
endif()
# these are not really 'test' in most cases they are experiments to try out an idea 
# or practice using a facility - such as json or strand
# x_test_request 		# same comment as test_x_client_request - not sure which one will survive
# x_test_ssl_server 	# boost server example implemented
# x_test_strand 		# experiment with strand
# x_test_tcp			# possibly will be devloped into a real test not sure its needed
# x_test_client_request # decome a real tests as more marvin/client/request* progresses 
# x_test_connect        # probably deprecated
# x_test_helpers		# need to investigate whther the helpers any required anymore
# x_test_json			# expploring nlohmann_json
# x_test_proxy			# probably needs to be resurrected
# x_test_timer			# experiment with timers and weak references

