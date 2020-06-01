#ifndef test_case_defs_hpp
#define test_case_defs_hpp

#include <iostream>
#include <iterator>
#include <algorithm>
#include <vector>
#include <string>
#include <map>
#include <marvin/error/marvin_error.hpp>
#include <marvin/http/header_fields.hpp>
#include "testcase.hpp"

/**
* This class is a collection of testcase objects
*/
class TestcaseDefinitions
{
public:
    TestcaseDefinitions();
    ~TestcaseDefinitions();
    void add_case(Testcase aCase);
    Testcase            get_case(int index);
    Testcase*           get_case_ptr(int index);
    TestcaseResultType  get_result(int index);
    int                 number_of_testcases();
    std::vector<Testcase> cases;
private:
};

std::vector<std::string> make_header(std::string key, std::string value);

/**
* Create a set of testcases (a TestcaseDefinitions object) consiting of single messages with
* various buffer arrangements to test that MessageReader paser works correctly
* regardless of how the incoming data is buffered.
*/
std::vector<Testcase> tc_make_buffering();
std::vector<Testcase> tc_make_eof();
std::vector<Testcase> tc_make_hv();
std::vector<Testcase> tc_make_ssl();


#endif
