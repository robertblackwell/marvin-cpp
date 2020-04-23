add_executable(config config.cpp)

target_include_directories(config 
	PUBLIC SYSTEM ${MARVIN_INCLUDE_PATHS})

target_link_libraries(config PUBLIC ${MARVIN_LINK_LIBRARIES})

# target_precompile_headers(${TARGET} 
# 	REUSE_FROM 
# 	marvin_library)

