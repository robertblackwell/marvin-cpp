set(TARGET test_certificates)
add_executable(${TARGET} 
    test_certificates.cpp
)
target_include_directories(${TARGET} PUBLIC ./ ../ ${TEST_INCLUDE_DIRS})
target_precompile_headers(${TARGET} REUSE_FROM marvin_library)
target_link_libraries(${TARGET} PUBLIC marvin_library ${TEST_LINK_LIBS})

# add_test(
#     ${TARGET} ${TARGET}
#     WORKING_DIRECTORY /home/robert/marvin++
#     )
# wrap the test in a shell script to set the working directory
add_test(test_certificates ${CMAKE_CURRENT_SOURCE_DIR}/test.sh ${PROJECT_DIR})

set_tests_properties(${TARGET}
    PROPERTIES 
    ENVIRONMENT "MARVIN_HOME=${PROJECT_DIR}")
