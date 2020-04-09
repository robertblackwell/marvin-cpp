add_subdirectory(test_env_utils)
add_subdirectory(test_certificates)
# set(TARGET test_certificates)
# add_executable(${TARGET} 
# 	test_certificates.cpp
# )
# target_include_directories(${TARGET} PUBLIC ./ ../test_helpers ../test_server_client ${TEST_INCLUDE_DIRS})
# target_precompile_headers(${TARGET} REUSE_FROM marvin_library)
# target_link_libraries(${TARGET} PUBLIC marvin_library ${TEST_LINK_LIBS})

# add_test(${TARGET} ${TARGET})

# set_target_properties(test_certificates PROPERTIES XCODE_GENERATE_SCHEME ON)

# set_tests_properties(${TARGET}
#     PROPERTIES ENVIRONMENT "SSL_CERT_FILE=/usr/local/etc/openssl@1.1/cert.pem")
# set(MARVIN_CERT_STORE_PATH ${PROJECT_DIR}/config/cert_store)
# set_target_properties(${TARGET}
#     PROPERTIES 
#     XCODE_SCHEME_ENVIRONMENT
#     "SSL_CERT_FILE=/usr/local/etc/openssl@1.1/cert.pem;MARVIN_CERT_STORE_PATH=${MARVIN_CERT_STORE_PATH}"
#     )
