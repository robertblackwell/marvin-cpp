file(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
set(TARGET_SUP server_setup)
add_executable(${TARGET_SUP} 
	server_setup.cpp
)

target_precompile_headers(${TARGET_SUP} REUSE_FROM marvin_library)

add_test(${TARGET_SUP} ${TARGET_SUP})

target_include_directories(${TARGET_SUP} PUBLIC ../test_helpers ../test_server_client ${TEST_INCLUDE_DIRS})
target_link_libraries(${TARGET_SUP} PUBLIC marvin_library ${TEST_LINK_LIBS})

set_tests_properties(server_setup    PROPERTIES FIXTURES_SETUP    Server)
# set_tests_properties(server_cleanup  PROPERTIES FIXTURES_CLEANUP  Server)
