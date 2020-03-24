
set(TARGET_CUP server_cleanup)
add_executable(${TARGET_CUP} 
	server_cleanup.cpp
)

target_precompile_headers(${TARGET_CUP} REUSE_FROM marvin_library)

add_test(${TARGET_CUP} ${TARGET_CUP})

target_include_directories(${TARGET_CUP} PUBLIC ../test_helpers ../test_server_client ${TEST_INCLUDE_DIRS})
target_link_libraries(${TARGET_CUP} PUBLIC marvin_library ${TEST_LINK_LIBS})
# set_tests_properties(server_setup    PROPERTIES FIXTURES_SETUP    Server)
set_tests_properties(server_cleanup  PROPERTIES FIXTURES_CLEANUP  Server)
