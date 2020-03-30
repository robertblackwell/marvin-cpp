add_subdirectory(demo1)
add_subdirectory(demo2)
add_subdirectory(manual)

# set(TARGET_1 test_server_v3)
# add_executable(${TARGET_1} 
# 	main.cpp
# 	demonstration.cpp
# 	v3_handler.cpp
# 	server_v3_runner.cpp
# )
# target_include_directories(${TARGET_1} PUBLIC ./ ./bf ../test_helpers ../test_server_client ${TEST_INCLUDE_DIRS})
# target_compile_features(${TARGET_1} PRIVATE cxx_std_14)
# target_link_libraries(${TARGET_1} PUBLIC marvin_library ${TEST_LINK_LIBS})
# target_precompile_headers(${TARGET_2} REUSE_FROM marvin_library)

# add_test(${TARGET_2} ${TARGET_1})
# SET_TESTS_PROPERTIES(${TARGET_1}
#     PROPERTIES ENVIRONMENT "SSL_CERT_FILE=/usr/local/etc/openssl@1.1/cert.pem")

