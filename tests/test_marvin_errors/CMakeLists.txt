file(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
set(TARGET test_marvin_errors)
add_executable(${TARGET} 
	main.cpp
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

set_tests_properties(${TARGET}
    PROPERTIES 
    ENVIRONMENT 
    "TEST_SERVER_BASEURL=${TEST_SERVER_BASEURL}"
    )

