add_executable(config config.cpp)

set(CONFIG_INCLUDE_DIRS 
	${PROJECT_DIR} 
	${PROJECT_DIR}/vendor/include)

message("Config Include Dirs: ${CONFIG_INCLUDE_DIRS}")

target_include_directories(config PUBLIC SYSTEM ${CONFIG_INCLUDE_DIRS})

target_compile_features(config PRIVATE cxx_std_17)

set(CONFIG_LINK_LIBS 
	marvin_library 
	Threads::Threads 
	${CMAKE_DL_LIBS}
	${VENDOR_DIR}/lib/libcrypto.a 
	${VENDOR_DIR}/lib/libssl.a 
	${VENDOR_DIR}/lib/libboost_filesystem.a 
	${VENDOR_DIR}/lib/libboost_system.a
	${VENDOR_DIR}/lib/libcert_library.a
	${CMAKE_DL_LIBS}
	${DL_LIB_SO}
	stdc++fs
	)


target_link_libraries(config PUBLIC ${CONFIG_LINK_LIBS})

