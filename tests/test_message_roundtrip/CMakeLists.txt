file(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
set(TARGET test_message_roundtrip)
add_executable(${TARGET} 
	main.cpp
	echo_smart.cpp
	echo_smart.hpp
	chunked.cpp
	chunked.hpp
	chunked_error.cpp
	chunked_error.hpp
	any_response.cpp 
	any_response.hpp
)
target_include_directories(${TARGET} PUBLIC ./ ../test_helpers ../test_server_client ${TEST_INCLUDE_DIRS})
target_precompile_headers(${TARGET} REUSE_FROM marvin_library)
target_precompile_headers(${TARGET} REUSE_FROM marvin_library)
target_link_libraries(${TARGET} PUBLIC marvin_library ${TEST_LINK_LIBS})

add_test(${TARGET} ${TARGET})
SET_TESTS_PROPERTIES(${TARGET}
    PROPERTIES ENVIRONMENT "SSL_CERT_FILE=/usr/local/etc/openssl@1.1/cert.pem")

