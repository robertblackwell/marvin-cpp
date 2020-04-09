add_executable(config config.cpp)
target_include_directories(config PUBLIC ${HEADER_DIRS} ${PROJECT_DIR}/vendor/include)
target_compile_features(config PRIVATE cxx_std_14)
target_link_libraries(config PUBLIC 
	marvin_library 
	${PROJECT_DIR}/vendor/lib/libcrypto.a
	${PROJECT_DIR}/vendor/lib/libssl.a
	${PROJECT_DIR}/vendor/lib/libboost_filesystem.a
	)

