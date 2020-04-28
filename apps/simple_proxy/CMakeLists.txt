add_executable(handshake 
	handshake.cpp
	marvin_handshaker.cpp
	cert_handshaker.cpp
)

target_include_directories(handshake 
	PUBLIC SYSTEM ${MARVIN_INCLUDE_PATHS})

target_link_libraries(handshake PUBLIC ${MARVIN_LINK_LIBRARIES})

# target_precompile_headers(${TARGET} 
# 	REUSE_FROM 
# 	marvin_library)

