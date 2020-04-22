file(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
set(TARGET test_tcp)
add_executable(${TARGET} 
	tcp_main.cpp
)

target_precompile_headers(${TARGET} REUSE_FROM marvin_library)

add_test(${TARGET} ${TARGET})

target_include_directories(${TARGET} PUBLIC ../test_helpers ../test_server_client ${TEST_INCLUDE_DIRS})
target_link_libraries(${TARGET} PUBLIC marvin_library ${TEST_LINK_LIBS})
set_tests_properties(test_tcp PROPERTIES FIXTURES_REQUIRED Server)

set_tests_properties(${TARGET}
    PROPERTIES 
    ENVIRONMENT 
    "TEST_SERVER_BASEURL=${TEST_SERVER_BASEURL}"
    )


