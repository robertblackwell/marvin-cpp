#ifndef test_case_hpp
#define test_case_hpp

#include <iostream>
#include <iterator>
#include <algorithm>
#include <vector>
#include <string>
#include <map>

typedef std::vector<std::string> TestcaseType;
typedef std::string TestcaseResultType;

//-----------------------------------------------------------------------------------------------------
class Testcase
{
public:
    Testcase(
             std::vector<std::string> rawMessage,
             
             std::string result_first_line,
             std::vector<std::vector<std::string>> result_headers,
             std::string result_body
             );
    std::string
    lineAt(std::size_t ix);
    
    bool
    verifyFirstLine(std::string fl);
    
    bool
    verifyHeaders(std::map<std::string, std::string> h);
    
    bool
    verifyBody(std::string b);
    
    std::string
    result_first_line();
    
    std::map<std::string, std::string>
    result_headers();
    
    std::string
    result_body();
    
    std::string
    next();
    
    bool
    finished();
    
    bool
    isErrorCase();
    
    std::string
    caseResult();
    
    int                                     _index;
    std::vector<std::string>                _rawMessage;
    std::string                             _result_first_line;
    std::vector<std::vector<std::string>>   _result_headers_vec;
    std::map<std::string, std::string>      _result_headers;
    std::string                             _result_body;
};

//-----------------------------------------------------------------------------------------------------
class Testcases
{
public:
    Testcases();
    ~Testcases();
    Testcase            getCase(int index);
    TestcaseResultType  getResult(int index);
    int                 numberOfTestcases();
private:
    std::vector<Testcase> cases;
    void addCase(TestcaseType aCase);
    void addCase(Testcase aCase);
};

std::vector<std::string> makeHeader(std::string key, std::string value);


#endif