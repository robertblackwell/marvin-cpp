set(TARGET test_doctest)

find_program(BASH_PROGRAM bash)

MESSAGE("BASH_PROGRAM      ${BASH_PROGRAM}")

add_executable(${TARGET} 
	test_doctest.cpp
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

# add_test(${TARGET} ${TARGET})
add_test(test_2doctest ${BASH_PROGRAM} ${CMAKE_CURRENT_SOURCE_DIR}/test.sh)
# set_tests_properties(${TARGET} PROPERTIES WORKING_DIRECTORY /home/robert/Projects/marvin++/cmake-build-debug/tests)

