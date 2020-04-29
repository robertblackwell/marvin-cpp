
set(TARGET test_pipe)
add_executable(${TARGET} 
	test_pipe.cpp
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

    # -------------------------------------------------------

set(TARGET2 test_pipe_writer)
add_executable(${TARGET2} 
    test_pipe_writer.cpp
)
target_include_directories(${TARGET2} 
    PUBLIC 
        ../test_helpers ../test_server_client 
        ${MARVIN_INCLUDE_PATHS})

target_precompile_headers(${TARGET2} 
    REUSE_FROM 
    marvin_library)

target_link_libraries(${TARGET2} 
    PUBLIC marvin_library ${MARVIN_LINK_LIBRARIES})
    
    