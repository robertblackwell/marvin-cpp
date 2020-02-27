#ifndef test_case_defs_hpp
#define test_case_defs_hpp

#include <iostream>
#include <iterator>
#include <algorithm>
#include <vector>
#include <string>
#include <map>
#include <marvin/error/marvin_error.hpp>
#include <marvin/http/http_header.hpp>
#include "bb_testcase.hpp"

namespace body_buffering {

    /**
    * Create a set of testcases (a TestcaseDefinitions object) consiting of single messages with
    * various buffer arrangements to test that MessageReader paser works correctly
    * regardless of how the incoming data is buffered.
    */
    std::vector<body_buffering::Testcase> make_test_cases();
    std::vector<body_buffering::Testcase> make_eof_cases();
    std::vector<body_buffering::Testcase> make_timeout_cases();
}

#endif
