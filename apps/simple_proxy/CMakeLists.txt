add_executable(simple_proxy 
	simple_proxy.cpp
	ctl_thread.cpp

)

target_include_directories(simple_proxy 
	PUBLIC SYSTEM ${MARVIN_INCLUDE_PATHS})

target_link_libraries(simple_proxy PUBLIC ${MARVIN_LINK_LIBRARIES})

# target_precompile_headers(${TARGET} 
# 	REUSE_FROM 
# 	marvin_library)

add_executable(simple_proxy_cli 
	simple_proxy_cli.cpp

)

target_include_directories(simple_proxy_cli 
	PUBLIC SYSTEM ${MARVIN_INCLUDE_PATHS})

target_link_libraries(simple_proxy_cli PUBLIC ${MARVIN_LINK_LIBRARIES})
