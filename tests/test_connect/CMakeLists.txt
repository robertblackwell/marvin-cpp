set(TARGET test_connect)
add_executable(${TARGET} 
	test_connect.cpp
)
target_include_directories(${TARGET} PUBLIC ./ ../test_helpers ../test_server_client ${TEST_INCLUDE_DIRS})
target_compile_features(${TARGET} PRIVATE cxx_std_14)
target_link_libraries(${TARGET} PUBLIC marvin_library ${TEST_LINK_LIBS})

add_test(${TARGET} ${TARGET})

set_target_properties(test_connect PROPERTIES XCODE_GENERATE_SCHEME ON)

set_tests_properties(${TARGET}
    PROPERTIES ENVIRONMENT "SSL_CERT_FILE=/usr/local/etc/openssl@1.1/cert.pem")

set_target_properties(${TARGET}
    PROPERTIES 
    XCODE_SCHEME_ENVIRONMENT
    "SSL_CERT_FILE=/usr/local/etc/openssl@1.1/cert.pem;MARVIN_CERT_STORE_PATH=${MARVIN_CERT_STORE_PATH}"
    )
