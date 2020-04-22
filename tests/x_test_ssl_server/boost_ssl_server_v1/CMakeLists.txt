
message("MarvinCertStorePath ${MARVIN_CERT_STORE_PATH}")
set(TARGET boost_ssl_server_v1)
add_executable(${TARGET}
    boost_ssl_server_v1.cpp
)
target_include_directories(${TARGET} PUBLIC  ${TEST_INCLUDE_DIRS})
target_precompile_headers(${TARGET} REUSE_FROM marvin_library)
target_link_libraries(${TARGET} PUBLIC marvin_library ${TEST_LINK_LIBS})

# add_test(${TARGET} ${TARGET})

set_target_properties(${TARGET} PROPERTIES XCODE_GENERATE_SCHEME ON)

