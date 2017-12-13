#include <iostream>
#include <iterator>
#include <algorithm>
#include <vector>
#include <string>
#include <map>
#include "message.hpp"
#include "bb_testcase.hpp"
#include "bb_testcase_defs.hpp"
#include "rb_logger.hpp"
RBLOGGER_SETLEVEL(LOG_LEVEL_DEBUG)

using namespace body_buffering;

int
TestcaseDefinitions::number_of_testcases()
{
    LogDebug("");
    return (int)cases.size();
}

void
TestcaseDefinitions::add_case(Testcase acase)
{
    cases.push_back(acase);
}
Testcase
TestcaseDefinitions::get_case(int index)
{

    auto c = cases[index];
    return c;
}

std::vector<std::string> make_header(std::string key, std::string value){
    return std::vector<std::string>{key, value};
}

TestcaseDefinitions::~TestcaseDefinitions(){}
TestcaseDefinitions::TestcaseDefinitions(){}

/**
* Create a set of testcases (a TestcaseDefinitions object) consiting of single messages with
* various buffer arrangements to test that MessageReader paser works correctly
* regardless of how the incoming data is buffered.
*/
TestcaseDefinitions body_buffering::makeTestcaseDefinitions_01()
{
    TestcaseDefinitions tcases;
    // case 0
     tcases.add_case(
            Testcase(
                "index 0 - simple 200 body length 10",
                std::vector<std::string> {
                    "POST / HTTP/1.1\r\n",
                    "Host: ahost\r\n",
                    "Connection: close\r\n",
                    "Proxy-Connection: keep-alive\r\n",
                    "Content-length: 11\r\n\r\n",
                    "01234567890",
                    "eof"
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
                    {HttpHeader::Name::Connection,"close"},
                    {HttpHeader::Name::ProxyConnection,"keep-alive"},
                    {HttpHeader::Name::ContentLength,"11"}
                },
                 // expected body
                 std::string("01234567890")
            )
    );
//    // index 1 -- EOH is signalled by a blank line
    tcases.add_case(
            Testcase(
                "index 1 - simple 200 body length 11 no body data in header buffer",
                std::vector<std::string>  {
                    "POST / HTTP/1.1\r\n",
                    "Connection: keep-alive\r\n",
                    "Proxy-Connection: close\r\n",
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
    tcases.add_case(
            Testcase(
                "index 2 - 200 body length 10 SOME body data in header buffer",
                std::vector<std::string>  {
                    "POST / HTTP/1.1\r\n",
                    "Host: ahost\r\n",
                    "Connection: keep-alive\r\n",
                    "Proxy-Connection: close\r\n",
                    "Content-length: 10\r\n\r\nAB",
                    "CDEFGHIJ"
                },
                std::string("HTTP/1.1 201 OK 22Reason Phrase\r\n"),
                // expected status code
                200,
                // expect error code in onHeader
                Marvin::make_error_ok(),
                // expected headers
                std::map< std::string, std::string >{
                    {HttpHeader::Name::Host, "ahost"},
                    {HttpHeader::Name::Connection,"close"},
                    {HttpHeader::Name::ProxyConnection,"keep-alive"},
                    {HttpHeader::Name::ContentLength,"10"}
                },
                // body
                std::string("ABCDEFGHIJ")
         )
    );
 
    // 3 EOH and EOM at the same time
    tcases.add_case(
            Testcase(
                "index 3 - simple 200 body length 10 SOME body data in with black line buffer",
                std::vector<std::string>  {
                    "POST / HTTP/1.1\r\n",
                    "Host: ahost\r\n",
                    "Connection: close\r\n",
                    "Proxy-Connection: keep-alive\r\n",
                    "Content-length: 10",
                    "\r\n\r\nABCDEFGHIJ"
                },
                std::string("HTTP/1.1 201 OK 22Reason Phrase\r\n"),
                // expected status code
                200,
                // expect error code in onHeader
                Marvin::make_error_ok(),
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
    tcases.add_case(
        Testcase(
                "index 4 - 200 body chunked encoding NO body data in header buffer",
                std::vector<std::string> {
                    "POST / HTTP/1.1\r\n",
                    "Host: ahost\r\n",
                    "Connection: close\r\n",
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
                200,
                // expect error code in onHeader
                Marvin::make_error_ok(),
                // expected headers
                    std::map< std::string, std::string >{
                        {HttpHeader::Name::Host, "ahost"},
                        {HttpHeader::Name::Connection,"close"},
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
    tcases.add_case(
            Testcase(
                "index 5 - simple 200 body chunked encoding SOME body data in buffer with black line after header",
                std::vector<std::string> {
                    "POST / HTTP/1.1\r\n",
                    "Host: ahost\r\n",
                    "Connection: close\r\n",
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
                200,
                // expect error code in onHeader
                Marvin::make_error_ok(),
                 // expected headers
                std::map< std::string, std::string >{
                    {HttpHeader::Name::Host, "ahost"},
                    {HttpHeader::Name::Connection,"close"},
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
    tcases.add_case(
                Testcase(
                "index 6 - simple 200 body chunked encoding SOME body data in buffer with black line after header",
                std::vector<std::string> {
                    "POST / HTTP/1.1\r\n",
                    "Host: ahost\r\n",
                    "Connection: keep-alive\r\n",
                    "Proxy-Connection: close\r\n",
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
                200,
                // expect error code in onHeader
                Marvin::make_error_ok(),
                // expected headers
                    std::map< std::string, std::string >{
                        {HttpHeader::Name::Host, "ahost"},
                        {HttpHeader::Name::Connection,"close"},
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
    tcases.add_case(
                Testcase(
                "index 7 - simple 200 body chunked encoding SOME body data in buffer with black line after header",
                std::vector<std::string> {
                    "POST / HTTP/1.1\r\n",
                    "Host: ahost\r\n",
                    "Connection: keep-alive\r\n",
                    "Proxy-Connection: close\r\n",
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
                200,
                // expect error code in onHeader
                Marvin::make_error_ok(),
                // expected headers
                    std::map< std::string, std::string >{
                        {HttpHeader::Name::Host, "ahost"},
                        {HttpHeader::Name::Connection,"close"},
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
    return tcases;
}
TestcaseDefinitions body_buffering::makeTCS_eof()
{
    TestcaseDefinitions tcases;
     tcases.add_case(
            Testcase(
                "index 0 - terminate with eof(shutdown) - no message length",
                std::vector<std::string> {
                    "POST / HTTP/1.1\r\n",
                    "Host: ahost\r\n",
                    "Connection: close\r\n",
                    "Proxy-Connection: keep-alive\r\n",
                    "\r\n",
                    "01234567890",
                    "eof"
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
                    {HttpHeader::Name::Connection,"close"},
                    {HttpHeader::Name::ProxyConnection,"keep-alive"}
                },
                 // expected body
                 std::string("01234567890")
            )
    );
     tcases.add_case(
            Testcase(
                "index 1 - terminate with close - no message length",
                std::vector<std::string> {
                    "POST / HTTP/1.1\r\n",
                    "Host: ahost\r\n",
                    "Connection: close\r\n",
                    "Proxy-Connection: keep-alive\r\n",
                    "\r\n",
                    "01234567890",
                    "close"
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
                    {HttpHeader::Name::Connection,"close"},
                    {HttpHeader::Name::ProxyConnection,"keep-alive"}
                },
                 // expected body
                 std::string("01234567890")
            )
    );
     tcases.add_case(
            Testcase(
                "index 0 - terminate with eof - no message length and no body",
                std::vector<std::string> {
                    "POST / HTTP/1.1\r\n",
                    "Host: ahost\r\n",
                    "Connection: close\r\n",
                    "Proxy-Connection: keep-alive\r\n",
                    "\r\n",
                    "close"
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
                    {HttpHeader::Name::Connection,"close"},
                    {HttpHeader::Name::ProxyConnection,"keep-alive"}
                },
                 // expected body
                 std::string("")
            )
    );

    
    return tcases;
}

