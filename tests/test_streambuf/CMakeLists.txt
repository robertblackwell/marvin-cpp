
set(TARGET test_streambuf)
add_executable(${TARGET} 
	test_streambuf.cpp
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
    