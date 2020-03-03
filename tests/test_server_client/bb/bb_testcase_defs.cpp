#include <iostream>
#include <iterator>
#include <algorithm>
#include <vector>
#include <string>
#include <map>
#include <json/json.hpp>
#include<marvin/http/message_base.hpp>
#include "bb_testcase.hpp"
#include "bb_testcase_defs.hpp"
#include <marvin/external_src/rb_logger/rb_logger.hpp>
RBLOGGER_SETLEVEL(LOG_LEVEL_DEBUG)

using namespace body_buffering;

std::vector<body_buffering::Testcase> body_buffering::make_test_cases()
{
    std::vector<body_buffering::Testcase> tcases;    // case 0
     tcases.push_back(
            Testcase(
                "index 0 - simple 200 body length 10",
                std::vector<std::string> {
                    "POST / HTTP/1.1\r\n",
                    "Host: ahost\r\n",
                    "Connection: close\r\n",
                    "Proxy-Connection: close\r\n",
                    "Content-length: 11\r\n\r\n",
                    "01234567890"
                },
                // expected first line
                std::string("HTTP/1.1 200 OK 11Reason Phrase"),
                // expected status code
                200,
                // expect error code in onHeader
                Marvin::make_error_ok(),
                // expexted headers
                Marvin::Http::Headers::Initializer{
                    {Marvin::Http::Headers::Name::Host, "ahost"},
                    {Marvin::Http::Headers::Name::Connection,"close"},
                    {Marvin::Http::Headers::Name::ProxyConnection,"close"},
                    {Marvin::Http::Headers::Name::ContentLength,"11"}
                },
                 // expected body
                 std::string("01234567890")
            )
    );
//    // index 1 -- EOH is signalled by a blank line
    tcases.push_back(
            Testcase(
                "index 1 - simple 200 body length 11 no body data in header buffer",
                std::vector<std::string>  {
                    "POST / HTTP/1.1\r\n",
                    "Connection: close\r\n",
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
                    Marvin::Http::Headers::Initializer{
//                    {Marvin::Http::Headers::Name::Host, "ahost"},
                    {Marvin::Http::Headers::Name::Connection,"close"},
                    {Marvin::Http::Headers::Name::ProxyConnection,"close"},
                    {Marvin::Http::Headers::Name::ContentLength,"11"}
                },

                std::string("11234567890")                            // expected body
        )
    );
//    // 2 EOH comes with some body data
    tcases.push_back(
            Testcase(
                "index 2 - 200 body length 10 SOME body data in header buffer",
                std::vector<std::string>  {
                    "POST / HTTP/1.1\r\n",
                    "Host: ahost\r\n",
                    "Connection: close\r\n",
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
                    Marvin::Http::Headers::Initializer{
                    {Marvin::Http::Headers::Name::Host, "ahost"},
                    {Marvin::Http::Headers::Name::Connection,"close"},
                    {Marvin::Http::Headers::Name::ProxyConnection,"close"},
                    {Marvin::Http::Headers::Name::ContentLength,"10"}
                },
                // body
                std::string("ABCDEFGHIJ")
         )
    );
 
    // 3 EOH and EOM at the same time
    tcases.push_back(
            Testcase(
                "index 3 - simple 200 body length 10 SOME body data in with black line buffer",
                std::vector<std::string>  {
                    "POST / HTTP/1.1\r\n",
                    "Host: ahost\r\n",
                    "Connection: close\r\n",
                    "Proxy-Connection: close\r\n",
                    "Content-length: 10",
                    "\r\n\r\nABCDEFGHIJ"
                },
                std::string("HTTP/1.1 201 OK 22Reason Phrase\r\n"),
                // expected status code
                200,
                // expect error code in onHeader
                Marvin::make_error_ok(),
                // expected headers
                    Marvin::Http::Headers::Initializer{
                        {Marvin::Http::Headers::Name::Host, "ahost"},
                        {Marvin::Http::Headers::Name::Connection,"close"},
                        {Marvin::Http::Headers::Name::ProxyConnection,"close"},
                        {Marvin::Http::Headers::Name::ContentLength,"10"}
                    },
                //             },
                std::string("ABCDEFGHIJ")
        )
    );
    
//
//    // 4 Chunked with headers on a boundary
    tcases.push_back(
        Testcase(
                "index 4 - 200 body chunked encoding NO body data in header buffer",
                std::vector<std::string> {
                    "POST / HTTP/1.1\r\n",
                    "Host: ahost\r\n",
                    "Connection: close\r\n",
                    "Proxy-Connection: close\r\n",
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
                    Marvin::Http::Headers::Initializer{
                        {Marvin::Http::Headers::Name::Host, "ahost"},
                        {Marvin::Http::Headers::Name::Connection,"close"},
                        {Marvin::Http::Headers::Name::ProxyConnection,"close"},
                        {Marvin::Http::Headers::Name::TransferEncoding,"chunked"}
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
    tcases.push_back(
            Testcase(
                "index 5 - simple 200 body chunked encoding SOME body data in buffer with black line after header",
                std::vector<std::string> {
                    "POST / HTTP/1.1\r\n",
                    "Host: ahost\r\n",
                    "Connection: close\r\n",
                    "Proxy-Connection: close\r\n",
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
                    Marvin::Http::Headers::Initializer{
                    {Marvin::Http::Headers::Name::Host, "ahost"},
                    {Marvin::Http::Headers::Name::Connection,"close"},
                    {Marvin::Http::Headers::Name::ProxyConnection,"close"},
                    {Marvin::Http::Headers::Name::TransferEncoding,"chunked"}
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
    tcases.push_back(
                Testcase(
                "index 6 - simple 200 body chunked encoding SOME body data in buffer with black line after header",
                std::vector<std::string> {
                    "POST / HTTP/1.1\r\n",
                    "Host: ahost\r\n",
                    "Connection: close\r\n",
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
                    Marvin::Http::Headers::Initializer{
                        {Marvin::Http::Headers::Name::Host, "ahost"},
                        {Marvin::Http::Headers::Name::Connection,"close"},
                        {Marvin::Http::Headers::Name::ProxyConnection,"close"},
                        {Marvin::Http::Headers::Name::TransferEncoding,"chunked"}
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
    tcases.push_back(
                Testcase(
                "index 7 - simple 200 body chunked encoding SOME body data in buffer with black line after header",
                std::vector<std::string> {
                    "POST / HTTP/1.1\r\n",
                    "Host: ahost\r\n",
                    "Connection: close\r\n",
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
                    Marvin::Http::Headers::Initializer{
                        {Marvin::Http::Headers::Name::Host, "ahost"},
                        {Marvin::Http::Headers::Name::Connection,"close"},
                        {Marvin::Http::Headers::Name::ProxyConnection,"close"},
                        {Marvin::Http::Headers::Name::TransferEncoding,"chunked"}
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
std::vector<body_buffering::Testcase> body_buffering::make_eof_cases()
{
    std::vector<Testcase> tcases;
     tcases.push_back(
            Testcase(
                "index 0 - terminate with eof(shutdown) - no message length",
                std::vector<std::string> {
                    "POST / HTTP/1.1\r\n",
                    "Host: ahost\r\n",
                    "Connection: close\r\n",
                    "Proxy-Connection: close\r\n",
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
                    Marvin::Http::Headers::Initializer{
                    {Marvin::Http::Headers::Name::Host, "ahost"},
                    {Marvin::Http::Headers::Name::Connection,"close"},
                    {Marvin::Http::Headers::Name::ProxyConnection,"close"}
                },
                 // expected body
                 std::string("01234567890")
            )
    );
     tcases.push_back(
            Testcase(
                "index 1 - terminate with close - no message length",
                std::vector<std::string> {
                    "POST / HTTP/1.1\r\n",
                    "Host: ahost\r\n",
                    "Connection: close\r\n",
                    "Proxy-Connection: close\r\n",
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
                    Marvin::Http::Headers::Initializer{
                    {Marvin::Http::Headers::Name::Host, "ahost"},
                    {Marvin::Http::Headers::Name::Connection,"close"},
                    {Marvin::Http::Headers::Name::ProxyConnection,"close"}
                },
                 // expected body
                 std::string("01234567890")
            )
    );
     tcases.push_back(
            Testcase(
                "index 0 - terminate with eof - no message length and no body",
                std::vector<std::string> {
                    "POST / HTTP/1.1\r\n",
                    "Host: ahost\r\n",
                    "Connection: close\r\n",
                    "Proxy-Connection: close\r\n",
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
                    Marvin::Http::Headers::Initializer{
                    {Marvin::Http::Headers::Name::Host, "ahost"},
                    {Marvin::Http::Headers::Name::Connection,"close"},
                    {Marvin::Http::Headers::Name::ProxyConnection,"close"}
                },
                 // expected body
                 std::string("")
            )
    );

    
    return tcases;
}
std::vector<body_buffering::Testcase> body_buffering::make_timeout_cases()
{
    std::vector<Testcase> tcases;
    nlohmann::json j;
    j["timeout"] = 10000;
    std::string body = j.dump();
    std::string len = std::to_string(body.size());
    /// this is the error code for timeout
    auto asio_op_aborted = boost::asio::error::make_error_code(boost::asio::error::operation_aborted);

     tcases.push_back(
            Testcase(
                "index 0 - timeout on waiting for response",
                std::vector<std::string> {
                    "POST /timeout HTTP/1.1\r\n",
                    "Host: ahost\r\n",
                    "Connection: close\r\n",
                    "Proxy-Connection: close\r\n",
                    "Content-Length: " + len + "\r\n"
                    "\r\n",
                    body
                },
                // expected first line
                std::string("HTTP/1.1 200 OK 11Reason Phrase"),
                // expected status code
                200,
                // expect error code in onHeader
                asio_op_aborted,
                // expexted headers
                    Marvin::Http::Headers::Initializer{
                    {Marvin::Http::Headers::Name::Host, "ahost"},
                    {Marvin::Http::Headers::Name::Connection,"close"},
                    {Marvin::Http::Headers::Name::ProxyConnection,"close"}
                },
                 // expected body
                 std::string("01234567890")
            )
    );
    return tcases;
}

