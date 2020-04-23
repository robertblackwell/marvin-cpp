
set(TARGET test_server_v3_manual)
add_executable(${TARGET} 
	manual.cpp
	../handle_app.cpp
	../handle_app.hpp
	../server_v3_runner.cpp
	../../test_message_roundtrip/any_response.cpp
	../../test_message_roundtrip/any_response.hpp
)
target_include_directories(${TARGET} PUBLIC 
	./ 
	../ 
	../../test_message_roundtrip 
	../../test_server_client 
	${TEST_INCLUDE_DIRS})
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

