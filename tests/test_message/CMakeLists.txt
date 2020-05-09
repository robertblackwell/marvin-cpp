
if(ON)
	set(TARGET test_message)
	add_executable(${TARGET} 
		main.cpp
		test_fill.cpp
		test_factory.cpp
		# test_parser.cpp

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
endif()

if(OFF)
	set(TARGET2 test_parser)

	add_executable(${TARGET2} 
		main.cpp
		test_parser.cpp
		test_new_parser.cpp
	
	)
	target_include_directories(${TARGET2} 
		PUBLIC 
			../test_helpers ../test_server_client 
			${MARVIN_INCLUDE_PATHS})
	
	target_precompile_headers(${TARGET2} 
		REUSE_FROM 
		marvin_library)
	
	target_link_libraries(${TARGET2} 
		PUBLIC marvin_library ${MARVIN_LINK_LIBRARIES})
	
	add_test(${TARGET2} ${TARGET2})
	SET_TESTS_PROPERTIES(${TARGET2}
		PROPERTIES ENVIRONMENT "SSL_CERT_FILE=/usr/local/etc/openssl@1.1/cert.pem")
endif()