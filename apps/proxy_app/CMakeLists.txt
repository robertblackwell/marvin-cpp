add_executable(proxy_app
	proxy_app.cpp
	ctl_thread.cpp

)

target_include_directories(proxy_app
	PUBLIC SYSTEM ${MARVIN_INCLUDE_PATHS})

target_link_libraries(proxy_app PUBLIC ${MARVIN_LINK_LIBRARIES})

