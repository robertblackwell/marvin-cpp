
message("MarvinCertStorePath ${MARVIN_CERT_STORE_PATH}")
set(TARGET test_ssl)
add_executable(test_ssl
    test_ssl.cpp
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

add_test(test_ssl test_ssl)

set_target_properties(test_ssl PROPERTIES XCODE_GENERATE_SCHEME ON)

set_tests_properties(test_ssl
    PROPERTIES 
    ENVIRONMENT 
    "SSL_CERT_FILE=/usr/local/etc/openssl@1.1/cert.pem;MARVIN_CERT_STORE_PATH=${MARVIN_CERT_STORE_PATH}"
    )
set_target_properties(test_ssl
    PROPERTIES 
    ENVIRONMENT 
    "SSL_CERT_FILE=/usr/local/etc/openssl@1.1/cert.pem;MARVIN_CERT_STORE_PATH=${MARVIN_CERT_STORE_PATH}"
    )
set_target_properties(test_ssl
    PROPERTIES 
    XCODE_SCHEME_ENVIRONMENT
    "SSL_CERT_FILE=/usr/local/etc/openssl@1.1/cert.pem;MARVIN_CERT_STORE_PATH=${MARVIN_CERT_STORE_PATH}"
    )

get_target_property(TPXC test_ssl XCODE_SCHEME_ENVIRONMENT)
message("test_ssl XCode Scheme Env ${TPXC}")

get_target_property(TP test_ssl ENVIRONMENT)
message("test_ssl Target Env ${TP}")


get_test_property(test_ssl ENVIRONMENT TSTP )
message("test_ssl Tests Env ${TSTP}")
# set_tests_properties(${TARGET}
# 	PROPERTIES
# 	PASS_REGULAR_EXPRESSION "Test passed")
# set_tests_properties(${TARGET}
# 	PROPERTIES
# 	FAIL_REGULAR_EXPRESSION "(Exception|Test failed)")
# set_tests_properties(${TARGET}
# 	PROPERTIES
# 	TIMEOUT 120)