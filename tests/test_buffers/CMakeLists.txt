
set(TARGET test_buffers)
add_executable(${TARGET} 
	test_buffers.cpp
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

add_test(${TARGET} ${TARGET})
SET_TESTS_PROPERTIES(${TARGET}
    PROPERTIES ENVIRONMENT "SSL_CERT_FILE=/usr/local/etc/openssl@1.1/cert.pem")



set(TARGET_T test_buffer_template)
add_executable(${TARGET_T}
		test_buffer_template.cpp
		)

target_include_directories(${TARGET_T}
		PUBLIC
		../test_helpers ../test_server_client
		${MARVIN_INCLUDE_PATHS})

target_precompile_headers(${TARGET_T}
		REUSE_FROM
		marvin_library)

target_link_libraries(${TARGET_T}
		PUBLIC marvin_library ${MARVIN_LINK_LIBRARIES})

add_test(${TARGET_T} ${TARGET_T})
SET_TESTS_PROPERTIES(${TARGET_T}
		PROPERTIES ENVIRONMENT "SSL_CERT_FILE=/usr/local/etc/openssl@1.1/cert.pem")
