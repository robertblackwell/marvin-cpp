#ifndef test_case_hpp
#define test_case_hpp

#include <iostream>
#include <iterator>
#include <algorithm>
#include <vector>
#include <string>

typedef std::vector<std::string> TestcaseType;
typedef std::string TestcaseResultType;

//-----------------------------------------------------------------------------------------------------
class Testcase
{
public:
    Testcase(
             std::string _first_line,
             std::vector<std::string> headers,
             std::vector<std::string> body,
             std::string result,
             bool raw=false
             )
    : first_line(_first_line),
    header_lines(headers),
    body_chunks(body),
    _result(result),
    _raw(raw)
    {
        
    }
    std::string lineAt(std::size_t ix)
    {
        std::string line;
        if( _index == 0){
            line = first_line;
            if( ! _raw ) line += "\r\n";
        }else if( (_index > 0) && (_index < header_lines.size() + 1 ) ){
            line = header_lines[_index - 1];
            if( ! _raw ) line += "\r\n";
        }else if( (_index > 0) && (_index < body_chunks.size() + header_lines.size() + 1) ){
            std::size_t  i = _index - 1 - header_lines.size();
            line = body_chunks[i];
        }
        return line;
        
    }
    std::string result(){
        return _result;
    }
    std::string next()
    {
        std::string line = lineAt(_index);
        _index++;
        return line;
    }
    bool finished()
    {
        bool r = (_index >= (1 + header_lines.size() + body_chunks.size()) );
        return r;
    }
    bool isErrorCase(){return false;}
    std::string caseResult(){ return "";}

    int                         _index;
    bool                        _raw;
    std::string                 first_line;
    std::vector<std::string>    header_lines;
    std::vector<std::string>    body_chunks;
    std::string                 _result;
};

//-----------------------------------------------------------------------------------------------------
class Testcases
{
public:
    Testcases();
    ~Testcases(){}
    Testcase            getCase(int index);
    TestcaseResultType  getResult(int index);
    int                 numberOfTestcases();
private:
    std::vector<Testcase> cases;
    void addCase(TestcaseType aCase);
    void addCase(Testcase aCase);
};
int Testcases::numberOfTestcases(){
    return (int)cases.size();
}
void Testcases::addCase(TestcaseType){}
void Testcases::addCase(Testcase acase)
{
    cases.push_back(acase);
}
Testcase Testcases::getCase(int index)
{
    return cases[index];
}
Testcases::Testcases(){
 
    // case 0
     addCase(
            Testcase(
                std::string("HTTP/1.1 200 OK 11Reason Phrase"),
                std::vector<std::string> {
                    "Host: ahost",
                    "Connection: keep-alive",
                    "Proxy-Connection: keep-alive",
                    "Content-length: 10",
                    ""
                },
                std::vector<std::string>{
                    "1234567890"
                },
                std::string("HTTP/1.1 200 OK 11Reason Phrase\r\n")
                + std::string("Host: ahost\r\n")
                + std::string("Connection: keep-alive\r\n")
                + std::string("Proxy-Connection: keep-alive\r\n")
                + std::string("Content-length: 10\r\n\r\n")
                + std::string("1234567890"),
                false
                )
            );
    // index 1 -- EOH is signalled by a blank line
    addCase(
            Testcase(
                std::string("HTTP/1.1 200 OK 11Reason Phrase\r\n"),
                std::vector<std::string>  {
                    "Host: ahost\r\n",
                    "Connection: keep-alive\r\n",
                    "Proxy-Connection: keep-alive\r\n",
                    "Content-length: 10\r\n",
                    "\r\n"},
                std::vector<std::string> {"1234567890"},
                std::string("HTTP/1.1 200 OK 11Reason Phrase\r\n")
                + std::string("Host: ahost\r\n")
                + std::string("Connection: keep-alive\r\n")
                + std::string("Proxy-Connection: keep-alive\r\n")
                + std::string("Content-length: 10\r\n\r\n")
                + std::string("1234567890"),
                  
                true)
            );
    // 2 EOH comes with some body data
    addCase(
            Testcase(
                std::string("HTTP/1.1 201 OK 22Reason Phrase\r\n"),
                std::vector<std::string>  {
                    "Host: ahost\r\n",
                    "Connection: keep-alive\r\n",
                    "Proxy-Connection: keep-alive\r\n",
                    "Content-length: 10\r\n\r\nAB"
                },
                std::vector<std::string>{"CDEFGHIJ"},
                std::string("HTTP/1.1 201 OK 22Reason Phrase\r\n")
                + std::string("Host: ahost\r\n")
                + std::string("Connection: keep-alive\r\n")
                + std::string("Proxy-Connection: keep-alive\r\n")
                + std::string("Content-length: 10\r\n\r\n")
                + std::string("ABCDEFGHIJ"),
                true)
             );
    
    // 3 EOH and EOM at the same time
    addCase(
            Testcase(
            std::string("HTTP/1.1 201 OK 22Reason Phrase\r\n"),
            std::vector<std::string>  {
                "Host: ahost\r\n",
                "Connection: keep-alive\r\n",
                "Proxy-Connection: keep-alive\r\n",
                "Content-length: 10",
                "\r\n\r\nABCDEFGHIJ"
            },
            std::vector<std::string>{""},
            std::string("HTTP/1.1 201 OK 22Reason Phrase\r\n")
            + std::string("Host: ahost\r\n")
            + std::string("Connection: keep-alive\r\n")
            + std::string("Proxy-Connection: keep-alive\r\n")
            + std::string("Content-length: 10\r\n\r\n")
            + std::string("ABCDEFGHIJ"),
            true)
            );
    
    // 4 Chunked with headers on a boundary
    addCase(
        Testcase(
            std::string("HTTP/1.1 201 OK Reason Phrase\r\n"),
            std::vector<std::string> {
                "Host: ahost\r\n",
                "Connection: keep-alive\r\n",
                "Proxy-Connection: keep-alive\r\n",
                "Transfer-encoding: chunked\r\n\r\n"
            },
             std::vector<std::string>{
                "0a\r\n1234567890\r\n",
                "0f\r\n1234567890XXXXX\r\n",
                "0a\r\n1234567890\r\n",
                "0f\r\n1234567890HGHGH\r\n",
                "0a\r\n1234567890\r\n",
                "0\r\n",
                "\r\n"
            },
            std::string("HTTP/1.1 201 OK Reason Phrase\r\n")
            + std::string("Host: ahost\r\n")
            + std::string("Connection: keep-alive\r\n")
            + std::string("Proxy-Connection: keep-alive\r\n")
            + std::string("Transfer-encoding: chunked\r\n\r\n")
            + std::string("0a\r\n1234567890\r\n")
            + std::string("0f\r\n1234567890XXXXX\r\n")
            + std::string("0a\r\n1234567890\r\n")
            + std::string("0f\r\n1234567890HGHGH\r\n")
            + std::string("0a\r\n1234567890\r\n")
            + std::string("0\r\n")
            + std::string("\r\n"),
            true)
        );
    
    // 5 Chunked with headers with some body data - chunks not broken
    addCase(
            Testcase(
                std::string("HTTP/1.1 201 OK Reason Phrase\r\n"),
                std::vector<std::string> {
                    "Host: ahost\r\n",
                    "Connection: keep-alive\r\n",
                    "Proxy-Connection: keep-alive\r\n",
                    "Transfer-Encoding: chunked\r\n"
                },
                std::vector<std::string>{
                    "\r\n0a\r\n1234567890\r\n",
                    "0f\r\n1234567890XXXXX\r\n",
                    "0a\r\n1234567890\r\n",
                    "0f\r\n1234567890HGHGH\r\n",
                    "0a\r\n1234567890\r\n",
                    "0\r\n",
                    "\r\n"
                },
                std::string("HTTP/1.1 201 OK Reason Phrase\r\n")
                + std::string("Host: ahost\r\n")
                + std::string("Connection: keep-alive\r\n")
                + std::string("Proxy-Connection: keep-alive\r\n")
                + std::string("Transfer-Encoding: chunked\r\n\r\n")
                + std::string("0a\r\n1234567890\r\n")
                + std::string("0f\r\n1234567890XXXXX\r\n")
                + std::string("0a\r\n1234567890\r\n")
                + std::string("0f\r\n1234567890HGHGH\r\n")
                + std::string("0a\r\n1234567890\r\n")
                + std::string("0\r\n")
                + std::string("\r\n"),
                true)
            );
    // 6 Chunked with headers with some body data - BROKEN chunks
    addCase(
        Testcase(
            std::string("HTTP/1.1 201 OK Reason Phrase\r\n"),
            std::vector<std::string> {
                "Host: ahost\r\n",
                "Connection: keep-alive\r\n",
                "Proxy-Connection: keep-alive\r\n",
                "Transfer-Encoding: chunked\r\n"
            },
             std::vector<std::string>{
                "\r\n0a\r\n123456",
                "7890\r\n",
                "0f\r\n123456",
                "7890XXXXX\r\n0a\r\n1234567890\r\n",
                "0f\r\n1234567890HGHGH\r\n",
                "0a\r\n1234567890\r\n",
                "0\r\n",
                "\r\n"
             },
             std::string("HTTP/1.1 201 OK Reason Phrase\r\n")
             + std::string("Host: ahost\r\n")
             + std::string("Connection: keep-alive\r\n")
             + std::string("Proxy-Connection: keep-alive\r\n")
             + std::string("Transfer-Encoding: chunked\r\n\r\n")
             + std::string("0a\r\n1234567890\r\n")
             + std::string("0f\r\n1234567890XXXXX\r\n")
             + std::string("0a\r\n1234567890\r\n")
             + std::string("0f\r\n1234567890HGHGH\r\n")
             + std::string("0a\r\n1234567890\r\n")
             + std::string("0\r\n")
             + std::string("\r\n"),
             true)
        );
    
//        addCase(
//            // 7 error
//            std::vector<std::string> {
//                "HTTP/1.1 200 OK 11Reason Phrase\r\n",
//                "Host: ahost\r\n",
//                "Connection: keep-alive\r\n",
//                "Proxy-Connection: keep-alive\r\n",
//                "Content-length: 10\r\n\r\n",
//                "1234567"
//            });
    }

#endif