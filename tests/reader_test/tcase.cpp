#include <iostream>
#include <iterator>
#include <algorithm>
#include <vector>
#include <string>
#include <map>
#include "tcase.hpp"

Testcase::Testcase(
    std::string description,
    std::vector<std::string> rawMessage,
    std::string result_first_line,
    int status_code,
    Marvin::ErrorType err,
    std::map<std::string, std::string> result_headers,
    std::string result_body
)
    :
    _rawMessage(rawMessage),
    _result_first_line(result_first_line),
    _result_headers_vec(result_headers),
    _result_body(result_body)
    {
        _result_headers = _result_headers_vec;
        _description = description;
        _index = 0;
        _result_status_code = status_code;
        _result_onheaders_err = err;
    }
    std::string Testcase::getDescription()
    {
        return _description;
    }
    std::string Testcase::lineAt(std::size_t ix)
    {
        std::string line = _rawMessage[ix];
        return line;
    }
    bool Testcase::verify_first_line(std::string fl)
    {
        return (fl == _result_first_line);
    }
    bool Testcase::verify_headers(std::map<std::string, std::string> h)
    {
        return (_result_headers == h);
    }
    bool Testcase::verify_body(std::string b)
    {
        return (b == _result_body);
    }
    std::string Testcase::result_first_line(){
        return _result_first_line;
    }
    int Testcase::result_status_code(){return _result_status_code;}

    Marvin::ErrorType Testcase::result_onheaders_err(){ return _result_onheaders_err; }

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
    bool Testcase::is_error_case(){return false;}

    std::string case_result(){ return "";}


//-----------------------------------------------------------------------------------------------------

int
Testcases::number_of_testcases(){
    return (int)cases.size();
}
void
Testcases::add_case(TestcaseType){}
void
Testcases::add_case(Testcase acase)
{
    cases.push_back(acase);
}
Testcase
Testcases::get_case(int index)
{
    return cases[index];
}

std::vector<std::string> make_header(std::string key, std::string value){
    return std::vector<std::string>{key, value};
}

Testcases::~Testcases(){}
Testcases::Testcases(){
    // case 0
     add_case(
            Testcase(
                "index 0 - simple 200 body length 10",
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
                // expected status code
                200,
                // expect error code in onHeader
                Marvin::make_error_ok(),
                // expexted headers
                std::map< std::string, std::string >{
                    {HttpHeader::Name::Host, "ahost"},
                    {HttpHeader::Name::Connection,"keep-alive"},
                    {HttpHeader::Name::ProxyConnection,"keep-alive"},
                    {HttpHeader::Name::ContentLength,"11"}
                },
                 // expected body
                 std::string("01234567890")
            )
    );
//    // index 1 -- EOH is signalled by a blank line
    add_case(
            Testcase(
                "index 1 - simple 200 body length 11 no body data in header buffer",
                std::vector<std::string>  {
                    "HTTP/1.1 200 OK 11Reason Phrase\r\n",
                    "Connection: keep-alive\r\n",
                    "Proxy-Connection: keep-alive\r\n",
                    "Content-length: 11\r\n",
                    "\r\n",
                    "11234567890"
                },
                std::string("HTTP/1.1 200 OK 11Reason Phrase\r\n"),   //expected first line
                // expected status code
                200,
                // expect error code in onHeader
                Marvin::make_error_ok(),
                // expexted headers
                std::map< std::string, std::string >{
//                    {HttpHeader::Name::Host, "ahost"},
                    {HttpHeader::Name::Connection,"keep-alive"},
                    {HttpHeader::Name::ProxyConnection,"keep-alive"},
                    {HttpHeader::Name::ContentLength,"11"}
                },

                std::string("11234567890")                            // expected body
        )
    );
//    // 2 EOH comes with some body data
    add_case(
            Testcase(
                "index 2 - 201 body length 10 SOME body data in header buffer",
                std::vector<std::string>  {
                    "HTTP/1.1 201 OK 22Reason Phrase\r\n",
                    "Host: ahost\r\n",
                    "Connection: keep-alive\r\n",
                    "Proxy-Connection: keep-alive\r\n",
                    "Content-length: 10\r\n\r\nAB",
                    "CDEFGHIJ"
                },
                std::string("HTTP/1.1 201 OK 22Reason Phrase\r\n"),
                // expected status code
                201,
                // expect error code in onHeader
                Marvin::make_error_ok(),
                // expected headers
                std::map< std::string, std::string >{
                    {HttpHeader::Name::Host, "ahost"},
                    {HttpHeader::Name::Connection,"keep-alive"},
                    {HttpHeader::Name::ProxyConnection,"keep-alive"},
                    {HttpHeader::Name::ContentLength,"10"}
                },
                // body
                std::string("ABCDEFGHIJ")
         )
    );
 
    // 3 EOH and EOM at the same time
    add_case(
            Testcase(
                "index 3 - simple 201 body length 10 SOME body data in with black line buffer",
                std::vector<std::string>  {
                    "HTTP/1.1 201 OK 22Reason Phrase\r\n",
                    "Host: ahost\r\n",
                    "Connection: keep-alive\r\n",
                    "Proxy-Connection: keep-alive\r\n",
                    "Content-length: 10",
                    "\r\n\r\nABCDEFGHIJ"
                },
                std::string("HTTP/1.1 201 OK 22Reason Phrase\r\n"),
                // expected status code
                201,
                // expect error code in onHeader
                Marvin::make_error_eom(),
                // expected headers
                    std::map< std::string, std::string >{
                        {HttpHeader::Name::Host, "ahost"},
                        {HttpHeader::Name::Connection,"keep-alive"},
                        {HttpHeader::Name::ProxyConnection,"keep-alive"},
                        {HttpHeader::Name::ContentLength,"10"}
                    },
                //             },
                std::string("ABCDEFGHIJ")
        )
    );
    
//
//    // 4 Chunked with headers on a boundary
    add_case(
        Testcase(
                "index 4 - 201 body chunked encoding NO body data in header buffer",
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
                // expected status code
                201,
                // expect error code in onHeader
                Marvin::make_error_ok(),
                // expected headers
                    std::map< std::string, std::string >{
                        {HttpHeader::Name::Host, "ahost"},
                        {HttpHeader::Name::Connection,"keep-alive"},
                        {HttpHeader::Name::ProxyConnection,"keep-alive"},
                        {HttpHeader::Name::TransferEncoding,"chunked"}
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
    add_case(
            Testcase(
                "index 5 - simple 201 body chunked encoding SOME body data in buffer with black line after header",
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
                // expected status code
                201,
                // expect error code in onHeader
                Marvin::make_error_ok(),
                 // expected headers
                std::map< std::string, std::string >{
                    {HttpHeader::Name::Host, "ahost"},
                    {HttpHeader::Name::Connection,"keep-alive"},
                    {HttpHeader::Name::ProxyConnection,"keep-alive"},
                    {HttpHeader::Name::TransferEncoding,"chunked"}
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
    add_case(
                Testcase(
                "index 6 - simple 201 body chunked encoding SOME body data in buffer with black line after header",
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
                // expected status code
                201,
                // expect error code in onHeader
                Marvin::make_error_ok(),
                // expected headers
                    std::map< std::string, std::string >{
                        {HttpHeader::Name::Host, "ahost"},
                        {HttpHeader::Name::Connection,"keep-alive"},
                        {HttpHeader::Name::ProxyConnection,"keep-alive"},
                        {HttpHeader::Name::TransferEncoding,"chunked"}
                    },
                // body
                std::string("1234567890")
                + std::string("1234567890XXXXX")
                + std::string("1234567890")
                + std::string("1234567890HGHGH")
                + std::string("1234567890")
                )
    );

//    // 7 Chunked with headers with some body data - BROKEN chunks
    add_case(
                Testcase(
                "index 7 - simple 201 body chunked encoding SOME body data in buffer with black line after header",
                std::vector<std::string> {
                    "HTTP/1.1 201 OK Reason Phrase\r\n",
                    "Host: ahost\r\n",
                    "Connection: keep-alive\r\n",
                    "Proxy-Connection: keep-alive\r\n",
                    "Transfer-Encoding: chunked\r\n"
                    "\r\n",
                    "0a\r\n123456",
                    "7890\r\n",
                    "0f\r\n123456",
                    "7890XXXXX\r\n0a\r\n1234567890\r\n",
                    "0f\r\n1234567890HGHGH\r\n",
                    "0a\r\n1234567890\r\n",
                    "0\r\n",
                    "\r\n"
                 },
                std::string("HTTP/1.1 201 OK Reason Phrase\r\n"),
                // expected status code
                201,
                // expect error code in onHeader
                Marvin::make_error_ok(),
                // expected headers
                    std::map< std::string, std::string >{
                        {HttpHeader::Name::Host, "ahost"},
                        {HttpHeader::Name::Connection,"keep-alive"},
                        {HttpHeader::Name::ProxyConnection,"keep-alive"},
                        {HttpHeader::Name::TransferEncoding,"chunked"}
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

