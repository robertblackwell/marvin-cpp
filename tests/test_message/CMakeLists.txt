set(TARGET_RDR test_message_reader)
add_executable(${TARGET_RDR} 
	main.cpp
	test_reader.cpp
	mock_socket.cpp
	repeating_timer.cpp
#	testcase.cpp
#	runner.cpp
#	testcase_defs.cpp

	helper_types.hpp
	data_single.hpp
	data_two.hpp
	data_eof.hpp
)
target_include_directories(${TARGET_RDR} 
	PUBLIC 
		./
		${MARVIN_INCLUDE_PATHS})

target_precompile_headers(${TARGET_RDR} 
	REUSE_FROM 
	marvin_library)

target_link_libraries(${TARGET_RDR} 
	PUBLIC marvin_library ${MARVIN_LINK_LIBRARIES})

add_test(${TARGET_RDR} ${TARGET_RDR})

if(ON)
	set(TARGET_MSG test_message)
	add_executable(${TARGET_MSG} 
		main.cpp
		test_fill.cpp
		test_factory.cpp

	)
	target_include_directories(${TARGET_MSG} 
		PUBLIC 
			./
			${MARVIN_INCLUDE_PATHS})

	target_precompile_headers(${TARGET_MSG} 
		REUSE_FROM 
		marvin_library)

	target_link_libraries(${TARGET_MSG} 
		PUBLIC marvin_library ${MARVIN_LINK_LIBRARIES})

	add_test(${TARGET_MSG} ${TARGET_MSG})

endif()

if(ON)
	set(TARGET2 test_parser)

	add_executable(${TARGET2} 
		main.cpp
		test_parser_2.cpp
		test_parser.cpp
	
	)
	target_include_directories(${TARGET2} 
		PUBLIC 
			./ 
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