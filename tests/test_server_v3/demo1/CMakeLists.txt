
set(TARGET test_server_v3_demo_1)
add_executable(${TARGET} 
	../main.cpp
	demo_1.cpp
	../handle_app.cpp
	../handle_app.hpp
	../server_v3_runner.cpp
)
target_include_directories(${TARGET} PUBLIC ./ ./bf ../test_helpers ../test_server_client ${TEST_INCLUDE_DIRS})
target_compile_features(${TARGET} PRIVATE cxx_std_14)
target_link_libraries(${TARGET} PUBLIC marvin_library ${TEST_LINK_LIBS})
target_precompile_headers(${TARGET} REUSE_FROM marvin_library)

# add_test(${TARGET} ${TARGET})
# SET_TESTS_PROPERTIES(${TARGET}
#     PROPERTIES ENVIRONMENT "SSL_CERT_FILE=/usr/local/etc/openssl@1.1/cert.pem")

