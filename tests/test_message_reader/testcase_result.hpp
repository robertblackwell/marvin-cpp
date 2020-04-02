//
//  testcase_result.hpp
//  test_marvin_errors
//
//  Created by ROBERT BLACKWELL on 11/30/17.
//  Copyright © 2017 Blackwellapps. All rights reserved.
//

#ifndef testcase_result_hpp
#define testcase_result_hpp

#include <iostream>
#include <iterator>
#include <algorithm>
#include <vector>
#include <string>
#include <map>
#include <marvin/error/marvin_error.hpp>
#include <marvin/http/headers_v2.hpp>
/**
* Class represents the reult of a single testcase message
*/
class TestcaseResult
{
public:
    TestcaseResult(
    std::string expected_first_line,
    int expected_status_code,
    Marvin::ErrorType expected_on_header_err,
    std::map<std::string, std::string> expected_headers,
    std::string expected_body
    Marvin::ErrorType expected_on_body_err,
);
    
    bool verify_first_line(std::string fl);
    
    bool verify_headers(std::map<std::string, std::string> h);
    
    bool verify_body(std::string b);
    
    std::string expected_first_line();
    
    int expected_status_code();
    
    Marvin::ErrorType expected_on_headers_err();
    Marvin::ErrorType expected_on_body_err();

    std::map<std::string, std::string> expected_headers();
    
    std::string expected_body();
    
    bool finished();
    
    bool is_error_case();
    
    std::string case_result();
    
    int                                     _result_status_code;
    Marvin::ErrorType                       _result_onheaders_err;
    std::string                             _result_first_line;
    std::map<std::string, std::string>      _result_headers_vec;
    std::map<std::string, std::string>      _result_headers;
    std::string                             _result_body;
};

#endif /* testcase_result_hpp */
