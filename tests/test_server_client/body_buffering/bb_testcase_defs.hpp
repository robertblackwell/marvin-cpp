#ifndef test_case_defs_hpp
#define test_case_defs_hpp

#include <iostream>
#include <iterator>
#include <algorithm>
#include <vector>
#include <string>
#include <map>
#include "marvin_error.hpp"
#include "http_header.hpp"
#include "bb_testcase.hpp"

namespace body_buffering {

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
        TestcaseResultType  get_result(int index);
        int                 number_of_testcases();
    private:
        std::vector<Testcase> cases;
    };

    std::vector<std::string> make_header(std::string key, std::string value);

    /**
    * Create a set of testcases (a TestcaseDefinitions object) consiting of single messages with
    * various buffer arrangements to test that MessageReaderV2 paser works correctly
    * regardless of how the incoming data is buffered.
    */
    TestcaseDefinitions makeTestcaseDefinitions_01();
    TestcaseDefinitions makeTCS_eof();
}

#endif
