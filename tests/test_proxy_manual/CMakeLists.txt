file(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
set(TARGET test_proxy_manual)
add_executable(${TARGET} 
	tp_main_manual.cpp
)
target_include_directories(${TARGET} PUBLIC ../test_helpers ../test_server_client ${TEST_INCLUDE_DIRS})
target_precompile_headers(${TARGET} REUSE_FROM marvin_library)
target_link_libraries(${TARGET} PUBLIC marvin_library ${TEST_LINK_LIBS})

