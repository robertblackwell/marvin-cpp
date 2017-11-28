#ifndef test_case_hpp
#define test_case_hpp

#include <iostream>
#include <iterator>
#include <algorithm>
#include <vector>
#include <string>
#include <map>
#include "marvin_error.hpp"
#include "http_header.hpp"

typedef std::vector<std::string> TestcaseType;
typedef std::string TestcaseResultType;

//-----------------------------------------------------------------------------------------------------
class Testcase
{
public:
    Testcase(
    std::string description,
    std::vector<std::string> rawMessage,
    std::string result_first_line,
    int status_code,
    Marvin::ErrorType result_onheader_err,
    std::map<std::string, std::string> result_headers,
    std::string result_body
);
    std::string getDescription();
    std::string lineAt(std::size_t ix);
    
    bool verify_first_line(std::string fl);
    
    bool verify_headers(std::map<std::string, std::string> h);
    
    bool verify_body(std::string b);
    
    std::string result_first_line();
    
    int result_status_code();
    
    Marvin::ErrorType result_onheaders_err();

    std::map<std::string, std::string> result_headers();
    
    std::string result_body();
    
    std::string next();
    
    bool finished();
    
    bool is_error_case();
    
    std::string case_result();
    
    int                                     _index;
    std::string                             _description;
    std::vector<std::string>                _rawMessage;
    int                                     _result_status_code;
    Marvin::ErrorType                       _result_onheaders_err;
    std::string                             _result_first_line;
    std::map<std::string, std::string>      _result_headers_vec;
    std::map<std::string, std::string>      _result_headers;
    std::string                             _result_body;
};

//-----------------------------------------------------------------------------------------------------
class Testcases
{
public:
    Testcases();
    ~Testcases();
    Testcase            get_case(int index);
    TestcaseResultType  get_result(int index);
    int                 number_of_testcases();
private:
    std::vector<Testcase> cases;
    void add_case(TestcaseType aCase);
    void add_case(Testcase aCase);
};

std::vector<std::string> make_header(std::string key, std::string value);


#endif
