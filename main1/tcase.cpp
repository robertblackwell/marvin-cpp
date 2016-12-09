#include <iostream>
#include <iterator>
#include <algorithm>
#include <vector>
#include <string>
#include <map>
#include "tcase.hpp"

Testcase::Testcase(
             std::vector<std::string> rawMessage,
             
             std::string result_first_line,
             std::vector<std::vector<std::string>> result_headers,
             std::string result_body
             )
    :
    _rawMessage(rawMessage),
    _result_first_line(result_first_line),
    _result_headers_vec(result_headers),
    _result_body(result_body)
    {
        _index = 0;
        for( auto const& entry: _result_headers_vec){
            _result_headers[entry[0]] = entry[1];
        }
    }
std::string Testcase::lineAt(std::size_t ix)
    {
        std::string line = _rawMessage[ix];
        return line;
    }
    bool Testcase::verifyFirstLine(std::string fl)
    {
        return (fl == _result_first_line);
    }
    bool Testcase::verifyHeaders(std::map<std::string, std::string> h)
    {
        return (_result_headers == h);
    }
    bool Testcase::verifyBody(std::string b)
    {
        return (b == _result_body);
    }
    std::string Testcase::result_first_line(){
        return _result_first_line;
    }
    std::map<std::string, std::string> Testcase::result_headers(){
        return _result_headers;
    }
    std::string Testcase::result_body(){
        return _result_body;
    }
    std::string Testcase::next()
    {
        std::string line = lineAt(_index);
        _index++;
        return line;
    }
    bool Testcase::finished()
    {
        bool r = (_index >= _rawMessage.size() );
        return r;
    }
    bool Testcase::isErrorCase(){return false;}
    std::string caseResult(){ return "";}


//-----------------------------------------------------------------------------------------------------

int
Testcases::numberOfTestcases(){
    return (int)cases.size();
}
void
Testcases::addCase(TestcaseType){}
void
Testcases::addCase(Testcase acase)
{
    cases.push_back(acase);
}
Testcase
Testcases::getCase(int index)
{
    return cases[index];
}

std::vector<std::string>
makeHeader(std::string key, std::string value){
    return std::vector<std::string>{key, value};
}

Testcases::~Testcases(){}
Testcases::Testcases(){
    // case 0
     addCase(
            Testcase(
                // raw message text
                std::vector<std::string> {
                    "HTTP/1.1 200 OK 11Reason Phrase",
                    "Host: ahost",
                    "Connection: keep-alive",
                    "Proxy-Connection: keep-alive",
                    "Content-length: 11",
                    "01234567890",
                    ""
                },
                // expected first line
                std::string("HTTP/1.1 200 OK 11Reason Phrase"),
                // expexted headers
                std::vector< std::vector< std::string > >{
                    std::vector<std::string>{"Host", "ahost"},
                    std::vector<std::string>{"Connection","keep-alive"},
                    std::vector<std::string>{"Proxy-Connection","keep-alive"},
                    std::vector<std::string>{"Content-length","11"}
                },
                
                 // expected body
                 std::string("01234567890")
                )
            );
//    // index 1 -- EOH is signalled by a blank line
    addCase(
            Testcase(
                std::vector<std::string>  {
                    "HTTP/1.1 200 OK 11Reason Phrase\r\n",
                    "Connection: keep-alive\r\n",
                    "Proxy-Connection: keep-alive\r\n",
                    "Content-length: 11\r\n",
                    "\r\n",
                    "11234567890"
                },
                std::string("HTTP/1.1 200 OK 11Reason Phrase\r\n"),   //expected first line
                // expexted headers
                std::vector< std::vector< std::string > >{
                    std::vector<std::string>{"Connection","keep-alive"},
                    std::vector<std::string>{"Proxy-Connection","keep-alive"},
                    std::vector<std::string>{"Content-length","11"}
                },

                std::string("11234567890")                            // expected body
        )
    );
//    // 2 EOH comes with some body data
    addCase(
            Testcase(
                std::vector<std::string>  {
                    "HTTP/1.1 201 OK 22Reason Phrase\r\n",
                    "Host: ahost\r\n",
                    "Connection: keep-alive\r\n",
                    "Proxy-Connection: keep-alive\r\n",
                    "Content-length: 10\r\n\r\nAB",
                    "CDEFGHIJ"
                },
                std::string("HTTP/1.1 201 OK 22Reason Phrase\r\n"),
                // expected headers
                std::vector< std::vector< std::string > >{
                    std::vector<std::string>{"Host", "ahost"},
                    std::vector<std::string>{"Connection","keep-alive"},
                    std::vector<std::string>{"Proxy-Connection","keep-alive"},
                    std::vector<std::string>{"Content-length","10"}
                },
                // body
                std::string("ABCDEFGHIJ")
         )
    );
 
    // 3 EOH and EOM at the same time
    addCase(
            Testcase(
            std::vector<std::string>  {
                "HTTP/1.1 201 OK 22Reason Phrase\r\n",
                "Host: ahost\r\n",
                "Connection: keep-alive\r\n",
                "Proxy-Connection: keep-alive\r\n",
                "Content-length: 10",
                "\r\n\r\nABCDEFGHIJ"
            },
            std::string("HTTP/1.1 201 OK 22Reason Phrase\r\n"),
            // expected headers
            std::vector< std::vector< std::string > >{
                 std::vector<std::string>{"Host", "ahost"},
                 std::vector<std::string>{"Connection","keep-alive"},
                 std::vector<std::string>{"Proxy-Connection","keep-alive"},
                 std::vector<std::string>{"Content-length","10"}
             },
            std::string("ABCDEFGHIJ")
        )
    );
    
//
//    // 4 Chunked with headers on a boundary
    addCase(
        Testcase(
            std::vector<std::string> {
                "HTTP/1.1 201 OK Reason Phrase\r\n",
                "Host: ahost\r\n",
                "Connection: keep-alive\r\n",
                "Proxy-Connection: keep-alive\r\n",
                "Transfer-Encoding: chunked\r\n\r\n"
                "0a\r\n1234567890\r\n",
                "0f\r\n1234567890XXXXX\r\n",
                "0a\r\n1234567890\r\n",
                "0f\r\n1234567890HGHGH\r\n",
                "0a\r\n1234567890\r\n",
                "0\r\n",
                "\r\n"
            },
            std::string("HTTP/1.1 201 OK Reason Phrase\r\n"),
            // expected headers
            std::vector< std::vector< std::string > >{
                std::vector<std::string>{"Host", "ahost"},
                std::vector<std::string>{"Connection","keep-alive"},
                std::vector<std::string>{"Proxy-Connection","keep-alive"},
                std::vector<std::string>{"Transfer-Encoding","chunked"}
            },

            // body
            std::string("1234567890")
            + std::string("1234567890XXXXX")
            + std::string("1234567890")
            + std::string("1234567890HGHGH")
            + std::string("1234567890")
        )
    );
//
//    // 5 Chunked with headers with some body data - chunks not broken
    addCase(
            Testcase(
                std::vector<std::string> {
                    "HTTP/1.1 201 OK Reason Phrase\r\n",
                    "Host: ahost\r\n",
                    "Connection: keep-alive\r\n",
                    "Proxy-Connection: keep-alive\r\n",
                    "Transfer-Encoding: chunked\r\n"
                    "\r\n0a\r\n1234567890\r\n",
                    "0f\r\n1234567890XXXXX\r\n",
                    "0a\r\n1234567890\r\n",
                    "0f\r\n1234567890HGHGH\r\n",
                    "0a\r\n1234567890\r\n",
                    "0\r\n",
                    "\r\n"
                },
                 std::string("HTTP/1.1 201 OK Reason Phrase\r\n"),
                 // expected headers
                 std::vector< std::vector< std::string > >{
                     std::vector<std::string>{"Host", "ahost"},
                     std::vector<std::string>{"Connection","keep-alive"},
                     std::vector<std::string>{"Proxy-Connection","keep-alive"},
                     std::vector<std::string>{"Transfer-Encoding","chunked"}
                 },
                     // body
                     std::string("1234567890")
                     + std::string("1234567890XXXXX")
                     + std::string("1234567890")
                     + std::string("1234567890HGHGH")
                     + std::string("1234567890")
        )
    );

//    // 6 Chunked with headers with some body data - BROKEN chunks
    addCase(
        Testcase(
            std::vector<std::string> {
                "HTTP/1.1 201 OK Reason Phrase\r\n",
                "Host: ahost\r\n",
                "Connection: keep-alive\r\n",
                "Proxy-Connection: keep-alive\r\n",
                "Transfer-Encoding: chunked\r\n"
                "\r\n0a\r\n123456",
                "7890\r\n",
                "0f\r\n123456",
                "7890XXXXX\r\n0a\r\n1234567890\r\n",
                "0f\r\n1234567890HGHGH\r\n",
                "0a\r\n1234567890\r\n",
                "0\r\n",
                "\r\n"
             },
            std::string("HTTP/1.1 201 OK Reason Phrase\r\n"),
            // expected headers
            std::vector< std::vector< std::string > >{
                std::vector<std::string>{"Host", "ahost"},
                std::vector<std::string>{"Connection","keep-alive"},
                std::vector<std::string>{"Proxy-Connection","keep-alive"},
                std::vector<std::string>{"Transfer-Encoding","chunked"}
            },
            // body
            std::string("1234567890")
            + std::string("1234567890XXXXX")
            + std::string("1234567890")
            + std::string("1234567890HGHGH")
            + std::string("1234567890")
            )
    );
}

