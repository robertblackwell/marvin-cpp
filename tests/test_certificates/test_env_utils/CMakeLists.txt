set(TARGET test_env_utils)
add_executable(${TARGET} 
    test_env_utils.cpp
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

set_target_properties(test_env_utils PROPERTIES XCODE_GENERATE_SCHEME ON)

set_tests_properties(${TARGET}
    PROPERTIES ENVIRONMENT "SSL_CERT_FILE=/usr/local/etc/openssl@1.1/cert.pem")
set(MARVIN_CERT_STORE_PATH ${PROJECT_DIR}/config/cert_store)
set_target_properties(${TARGET}
    PROPERTIES 
    XCODE_SCHEME_ENVIRONMENT
    "SSL_CERT_FILE=/usr/local/etc/openssl@1.1/cert.pem;MARVIN_CERT_STORE_PATH=${MARVIN_CERT_STORE_PATH}"
    )
