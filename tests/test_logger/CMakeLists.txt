
set(TARGET test_logger)
add_executable(${TARGET} 
	test_logger.cpp
)
target_include_directories(${TARGET} PUBLIC ../test_proxy ../test_helpers ../test_server_client ${TEST_INCLUDE_DIRS})
target_link_libraries(${TARGET} PUBLIC marvin_library ${TEST_LINK_LIBS})

add_test(${TARGET} ${TARGET})
