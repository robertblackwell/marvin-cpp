set(TARGET test_certificates)
add_executable(${TARGET} 
    test_certificates.cpp
)
target_include_directories(${TARGET} 
	PUBLIC 
		../test_helpers ../test_server_client 
		${MARVIN_INCLUDE_PATHS})

target_precompile_headers(${TARGET} 
	REUSE_FROM 
	marvin_library)

target_link_libraries(${TARGET} 
	PUBLIC ${MARVIN_LINK_LIBRARIES})

get_target_property(TLL ${TARGET} LINK_LIBRARIES)
message("test_certificates MARVIN_LINK_LIRARIES                                   ${MARVIN_LINK_LIBRARIES}")
message("test_certificates TARGET_LINK_LIRARIES                                   ${TLL}")

add_test(${TARGET} /bin/bash ${CMAKE_CURRENT_SOURCE_DIR}/test.sh ${MARVIN_PROJECT_DIR})
