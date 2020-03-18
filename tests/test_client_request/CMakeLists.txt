file(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
set(TARGET test_client_request_pipeline)
add_executable(${TARGET} 
	main.cpp
	pipeline.cpp
)
target_include_directories(${TARGET} PUBLIC ./ ../test_helpers ../test_server_client ${TEST_INCLUDE_DIRS})
target_link_libraries(${TARGET} PUBLIC marvin_library ${TEST_LINK_LIBS})

add_test(${TARGET} ${TARGET})
SET_TESTS_PROPERTIES(${TARGET}
    PROPERTIES ENVIRONMENT "SSL_CERT_FILE=/usr/local/etc/openssl@1.1/cert.pem")


set(TARGET2 test_client_request_roundtrip)
add_executable(${TARGET2} 
	main.cpp
	roundtrip.cpp
)
target_include_directories(${TARGET2} PUBLIC ./ ../test_helpers ../test_server_client ${TEST_INCLUDE_DIRS})
target_link_libraries(${TARGET2} PUBLIC marvin_library ${TEST_LINK_LIBS})

add_test(${TARGET2} ${TARGET2})
SET_TESTS_PROPERTIES(${TARGET}
    PROPERTIES ENVIRONMENT "SSL_CERT_FILE=/usr/local/etc/openssl@1.1/cert.pem")

set(TARGET3 test_client_request_multiple)
add_executable(${TARGET3} 
	main.cpp
	multiple.cpp
)
target_include_directories(${TARGET3} PUBLIC ./ ../test_helpers ../test_server_client ${TEST_INCLUDE_DIRS})
target_compile_features(${TARGET3} PRIVATE cxx_std_14)
target_link_libraries(${TARGET3} PUBLIC marvin_library ${TEST_LINK_LIBS})

add_test(${TARGET3} ${TARGET3})
SET_TESTS_PROPERTIES(${TARGET3}
    PROPERTIES ENVIRONMENT "SSL_CERT_FILE=/usr/local/etc/openssl@1.1/cert.pem")


# set_tests_properties(${TARGET}
# 	PROPERTIES
# 	PASS_REGULAR_EXPRESSION "Test passed")
# set_tests_properties(${TARGET}
# 	PROPERTIES
# 	FAIL_REGULAR_EXPRESSION "(Exception|Test failed)")
# set_tests_properties(${TARGET}
# 	PROPERTIES
# 	TIMEOUT 120)