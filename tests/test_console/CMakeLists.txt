
set(TARGET test_console)
add_executable(${TARGET} 
    test_main.cpp
)
target_include_directories(${TARGET} 
	PUBLIC 
		../test_helpers ../test_server_client 
		${MARVIN_INCLUDE_PATHS})

target_precompile_headers(${TARGET} 
	REUSE_FROM 
	marvin_library)

target_link_libraries(${TARGET} 
	PUBLIC marvin_library ${MARVIN_LINK_LIBRARIES})

#add_test(${TARGET} ${TARGET})
#SET_TESTS_PROPERTIES(${TARGET}
#    PROPERTIES ENVIRONMENT "SSL_CERT_FILE=/usr/local/etc/openssl@1.1/cert.pem")
#
#add_test(${TARGET} ${TARGET})
#SET_TESTS_PROPERTIES(${TARGET}
#    PROPERTIES ENVIRONMENT "SSL_CERT_FILE=/usr/local/etc/openssl@1.1/cert.pem")
#

# set_tests_properties(${TARGET}
# 	PROPERTIES
# 	PASS_REGULAR_EXPRESSION "Test passed")
# set_tests_properties(${TARGET}
# 	PROPERTIES
# 	FAIL_REGULAR_EXPRESSION "(Exception|Test failed)")
# set_tests_properties(${TARGET}
# 	PROPERTIES
# 	TIMEOUT 120)