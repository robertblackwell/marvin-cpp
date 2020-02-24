add_executable(proxy main.cpp)
target_include_directories(proxy PUBLIC ${HEADER_DIRS} ${PROJECT_DIR}/vendor/include)
target_compile_features(proxy PRIVATE cxx_std_14)
target_link_libraries(proxy PUBLIC 
	marvin_library 
	${PROJECT_DIR}/vendor/lib/libcrypto.a
	${PROJECT_DIR}/vendor/lib/libssl.a
	${PROJECT_DIR}/vendor/lib/libboost_filesystem.a
	)

