set(TARGET test_doctest)
add_executable(${TARGET} 
	test_doctest.cpp
)
target_include_directories(${TARGET} PUBLIC ./ ../test_helpers ../test_server_client ${TEST_INCLUDE_DIRS})
target_precompile_headers(${TARGET} REUSE_FROM marvin_library)
target_link_libraries(${TARGET} PUBLIC marvin_library ${TEST_LINK_LIBS})

add_test(${TARGET} ${TARGET})
add_test(test_2doctest ${CMAKE_CURRENT_SOURCE_DIR}/test.sh)
# set_tests_properties(${TARGET} PROPERTIES WORKING_DIRECTORY /home/robert/Projects/marvin++/cmake-build-debug/tests)

