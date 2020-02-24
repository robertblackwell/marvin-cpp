# CMake generated Testfile for 
# Source directory: /Users/robertblackwell/Projects/marvin++/tests/test_client_request
# Build directory: /Users/robertblackwell/Projects/marvin++/cmake-build/tests/test_client_request
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(test_client_request_pipeline "test_client_request_pipeline")
set_tests_properties(test_client_request_pipeline PROPERTIES  ENVIRONMENT "SSL_CERT_FILE=/usr/local/etc/openssl@1.1/cert.pem" _BACKTRACE_TRIPLES "/Users/robertblackwell/Projects/marvin++/tests/test_client_request/CMakeLists.txt;11;add_test;/Users/robertblackwell/Projects/marvin++/tests/test_client_request/CMakeLists.txt;0;")
add_test(test_client_request_roundtrip "test_client_request_roundtrip")
set_tests_properties(test_client_request_roundtrip PROPERTIES  _BACKTRACE_TRIPLES "/Users/robertblackwell/Projects/marvin++/tests/test_client_request/CMakeLists.txt;25;add_test;/Users/robertblackwell/Projects/marvin++/tests/test_client_request/CMakeLists.txt;0;")
add_test(test_client_request_multiple "test_client_request_multiple")
set_tests_properties(test_client_request_multiple PROPERTIES  ENVIRONMENT "SSL_CERT_FILE=/usr/local/etc/openssl@1.1/cert.pem" _BACKTRACE_TRIPLES "/Users/robertblackwell/Projects/marvin++/tests/test_client_request/CMakeLists.txt;38;add_test;/Users/robertblackwell/Projects/marvin++/tests/test_client_request/CMakeLists.txt;0;")
