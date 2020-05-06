#ifndef marvin_tests_parser_test_data_hpp
#define marvin_tests_parser_test_data_hpp

#include <iostream>
#include <iterator>
#include <algorithm>
#include <vector>
#include <boost/asio.hpp>
#include <boost/asio/basic_streambuf.hpp>
#include <doctest/doctest.h>
#include <marvin/http/message_base.hpp>
#include <marvin/http/parser.hpp>

#include "parser_helpers.hpp"

namespace Marvin { 
namespace Tests { 
namespace Parser { 

TestCollection parser_test_data() 
{
    static TestCollection ptc;
    TestSet ts;
    // ptc["back_to_back"] = TestSet{
    ptc["back_to_back"] = TestSet{
        "back_to_back_messages",
        std::vector<char*> {
            (char*) "HTTP/1.1 200 OK 11Reason Phrase\r\n\0        ",
            (char*) "Host: ahost\r\n",
            (char*) "Connection: keep-alive\r\n",
            (char*) "Proxy-Connection: keep\0    ",
            (char*) "-alive\r\n\0    ",
            (char*) "Content-length: 10\r\n\r\n",
            (char*) "1234567",
            (char*) "890HTTP/1.1 ",
            (char*) "201 OK 22Reason Phrase\r\n",
            (char*) "Host: ahost\r\n",
            (char*) "Connection: keep-alive\r\n",
            (char*) "Proxy-Connection: keep-alive\r\n",
            (char*) "Content-length: 11\r\n",
            (char*) "\r\n",
            (char*) "ABCDEFGHIJK\0      ",
            NULL
            },
        [](std::vector<Marvin::MessageBase*> messages)
        {
            REQUIRE(messages.size() > 0);
            Marvin::MessageBase* m0 = dynamic_cast<Marvin::MessageBase*>(messages[0]);
            REQUIRE(m0 != nullptr);
            CHECK(m0->httpVersMajor() == 1);
            CHECK(m0->httpVersMinor() == 1);
            CHECK(m0->statusCode() == 200);
            CHECK(m0->header("CONTENT-LENGTH") == "10");
            CHECK(m0->header("CONNECTION") == "keep-alive");
            CHECK(m0->header("PROXY-CONNECTION") == "keep-alive");
            auto b0 = m0->getContentBuffer()->to_string();
            CHECK(m0->getContent()->to_string() == "1234567890");
            Marvin::MessageBase* m1 = dynamic_cast<Marvin::MessageBase*>(messages[1]);
            REQUIRE(m1 != nullptr);
            CHECK(m1->httpVersMajor() == 1);
            CHECK(m1->httpVersMinor() == 1);
            CHECK(m1->statusCode() == 201);
            CHECK(m1->header("CONTENT-LENGTH") == "11");
            CHECK(m1->header("CONNECTION") == "keep-alive");
            CHECK(m1->header("PROXY-CONNECTION") == "keep-alive");
            auto b1 = m1->getContentBuffer()->to_string();
            CHECK(m1->getContent()->to_string() == "ABCDEFGHIJK");
        }
    };
    
    ptc["eof_data_set"] = {
        "eof_data_set",
        std::vector<char*>{
            (char*) "HTTP/1.1 200 OK 11Reason Phrase\r\n\0        ",
            (char*) "Host: ahost\r\n",
            (char*) "Connection: keep-alive\r\n",
            (char*) "Proxy-Connection: keep\0    ",
            (char*) "-alive\r\n\0    ",
            (char*) "\r\n",
            (char*) "1234567890",
            NULL
        },
        [](std::vector<Marvin::MessageBase*> messages)
        {
            Marvin::MessageBase* m0 = dynamic_cast<Marvin::MessageBase*>(messages[0]);
            CHECK(m0->httpVersMajor() == 1);
            CHECK(m0->httpVersMinor() == 1);
            CHECK(m0->statusCode() == 200);
            CHECK(m0->header("CONNECTION") == "keep-alive");
            CHECK(m0->header("PROXY-CONNECTION") == "keep-alive");
            auto b0 = m0->getContentBuffer()->to_string();
            CHECK(m0->getContent()->to_string() == "1234567890");

        }
    };

    ptc["simple_data_set"] = TestSet{
        "simple_data_set",
        std::vector<char*>{
        (char*) "GET /apath HTTP/1.1\r\nHost: ahost\r\n",
        (char*) "Connection: keep-alive\r\nProxy-Connection: keep-alive\r\n",
        (char*) "Content-Length: 10\r\n\r\n",
        (char*) "9123456789",
        NULL
        },
        [](std::vector<Marvin::MessageBase*> messages)
        {
                Marvin::MessageBase* msg_p = dynamic_cast<Marvin::MessageBase*>(messages[0]);
            CHECK(msg_p->httpVersMajor() == 1);
            CHECK(msg_p->httpVersMinor() == 1);
            CHECK(msg_p->getMethodAsString() == "GET");
            CHECK(msg_p->header("CONTENT-LENGTH") == "10");
            CHECK(msg_p->header("CONNECTION") == "keep-alive");
            CHECK(msg_p->header("PROXY-CONNECTION") == "keep-alive");
            auto b = msg_p->getContentBuffer()->to_string();
            CHECK(msg_p->getContent()->to_string() == "9123456789");
        }
    };
    
    ptc["chunked_overlap_boy"] = TestSet{
        "chunked_overlap_body",
        std::vector<char*>{
            (char*) "HTTP/1.1 201 OK Reason Phrase\r\n",
            (char*) "Host: ahost\r\n",
            (char*) "Connection: keep-alive\r\n",
            (char*) "Proxy-Connection: keep-alive\r\n",
            (char*) "Transfer-Encoding: chunked\r\n",
            (char*) "\r\n",
            (char*) "0a\r\n1234567890\r\n",
            (char*) "0a\r\n1234567890\r\n",
            (char*) "0a\r\n1234567890\r\n",
            (char*) "0a\r\n1234567890\r\n",
            (char*) "0a\r\n1234567890\r\n",
            (char*) "0a\r\n1234567890\r\n",
            (char*) "0f\r\n1234567",
            (char*) "890XXXXX\r\n",
            (char*) "0f\r\n1234567890YYYYY\r\n",
            (char*) "0a\r\n1234567890\r\n",
            (char*) "0a\r\n1234567890\r\n",
            (char*) "0f\r\n1234567890HGHGH\r\n",
            (char*) "0a\r\n1234567890\r\n",
            (char*) "0\r\n",
            (char*) "\r\n",
            NULL
        },
        [](std::vector<Marvin::MessageBase*> messages)
        {
            Marvin::MessageBase* msg_p = dynamic_cast<Marvin::MessageBase*>(messages[0]);
            CHECK(msg_p->httpVersMajor() == 1);
            CHECK(msg_p->httpVersMinor() == 1);
            CHECK(msg_p->statusCode() == 201);
            CHECK(! msg_p->hasHeader("CONTENT-LENGTH"));
            CHECK(msg_p->header("TRANSFER-ENCODING") == "chunked");
            CHECK(msg_p->header("CONNECTION") == "keep-alive");
            CHECK(msg_p->header("PROXY-CONNECTION") == "keep-alive");
            auto b = msg_p->getContentBuffer()->to_string();
            CHECK(msg_p->getContent()->to_string() ==
                (
                std::string("1234567890") +
                std::string("1234567890") +
                std::string("1234567890") +
                std::string("1234567890") +
                std::string("1234567890") +
                std::string("1234567890") +
                std::string("1234567890XXXXX") +
                std::string("1234567890YYYYY") +
                std::string("1234567890") +
                std::string("1234567890") +
                std::string("1234567890HGHGH") +
                std::string("1234567890"))
                );
        }
    };

    ptc["chunked_overlap_header_body_1"] = TestSet{
        "chunked_overlap_header_body_1",
        std::vector<char*>{
            (char*) "HTTP/1.1 201 OK Reason Phrase\r\n",
            (char*) "Host: ahost\r\n",
            (char*) "Connection: keep-alive\r\n",
            (char*) "Transfer-Encoding: chunked\r\n",
            (char*) "Proxy-Connection: keep-alive\r\n\r\n0a\r\n123",
            (char*) "4567890\r\n",
            (char*) "0a\r\n1234567890\r\n",
            (char*) "0a\r\n1234567890\r\n",
            (char*) "0a\r\n1234567890\r\n",
            (char*) "0a\r\n1234567890\r\n",
            (char*) "0a\r\n1234567890\r\n",
            (char*) "0f\r\n1234567890XXXXX\r\n",
            (char*) "0f\r\n1234567890YYYYY\r\n",
            (char*) "0a\r\n1234567890\r\n",
            (char*) "0a\r\n1234567890\r\n",
            (char*) "0f\r\n1234567890HGHGH\r\n",
            (char*) "0a\r\n1234567890\r\n",
            (char*) "0\r\n",
            (char*) "\r\n",
            NULL
        },
        [](std::vector<Marvin::MessageBase*> messages)
        {
            Marvin::MessageBase* msg_p = dynamic_cast<Marvin::MessageBase*>(messages[0]);
            CHECK(msg_p->httpVersMajor() == 1);
            CHECK(msg_p->httpVersMinor() == 1);
            CHECK(msg_p->statusCode() == 201);
            CHECK(! msg_p->hasHeader("CONTENT-LENGTH"));
            CHECK(msg_p->header("TRANSFER-ENCODING") == "chunked");
            CHECK(msg_p->header("CONNECTION") == "keep-alive");
            CHECK(msg_p->header("PROXY-CONNECTION") == "keep-alive");
            auto b = msg_p->getContentBuffer()->to_string();
            CHECK(msg_p->getContent()->to_string() ==
                (
                std::string("1234567890") +
                std::string("1234567890") +
                std::string("1234567890") +
                std::string("1234567890") +
                std::string("1234567890") +
                std::string("1234567890") +
                std::string("1234567890XXXXX") +
                std::string("1234567890YYYYY") +
                std::string("1234567890") +
                std::string("1234567890") +
                std::string("1234567890HGHGH") +
                std::string("1234567890"))
                );
        }
    };

    ptc["chunked_overlap_body_2"] = TestSet{
        "chunked_overlap_header_body_2",
        std::vector<char*>{
            (char*) "HTTP/1.1 201 OK Reason Phrase\r\n",
            (char*) "Host: ahost\r\n",
            (char*) "Connection: keep-alive\r\n",
            (char*) "Transfer-Encoding: chunked\r\n",
            (char*) "Proxy-Connection: keep-alive\r\n\r\n0a\r\n123",
            (char*) "4567890\r\n",
            (char*) "0a\r\n1234567890\r\n",
            (char*) "0a\r\n1234567890\r\n",
            (char*) "0a\r\n1234567890\r\n",
            (char*) "0a\r\n1234567890\r\n",
            (char*) "0a\r\n1234567890\r\n",
            (char*) "0f\r\n1234567890XXXXX\r\n",
            (char*) "0f\r\n1234567890YYYYY\r\n",
            (char*) "0a\r\n1234567890\r\n",
            (char*) "0a\r\n1234567890\r\n",
            (char*) "0f\r\n1234567890HGHGH\r\n",
            (char*) "0a\r\n1234567890\r\n",
            (char*) "0\r\n",
            (char*) "\r\n",
            NULL
        },
        [](std::vector<Marvin::MessageBase*> messages)
        {
            Marvin::MessageBase* msg_p = dynamic_cast<Marvin::MessageBase*>(messages[0]);
            CHECK(msg_p->httpVersMajor() == 1);
            CHECK(msg_p->httpVersMinor() == 1);
            CHECK(msg_p->statusCode() == 201);
            CHECK(! msg_p->hasHeader("CONTENT-LENGTH"));
            CHECK(msg_p->header("TRANSFER-ENCODING") == "chunked");
            CHECK(msg_p->header("CONNECTION") == "keep-alive");
            CHECK(msg_p->header("PROXY-CONNECTION") == "keep-alive");
            auto b = msg_p->getContentBuffer()->to_string();
            CHECK(msg_p->getContent()->to_string() ==
                (
                std::string("1234567890") +
                std::string("1234567890") +
                std::string("1234567890") +
                std::string("1234567890") +
                std::string("1234567890") +
                std::string("1234567890") +
                std::string("1234567890XXXXX") +
                std::string("1234567890YYYYY") +
                std::string("1234567890") +
                std::string("1234567890") +
                std::string("1234567890HGHGH") +
                std::string("1234567890"))
                );
        }
    };
    return ptc; 
}
} // namespace Parser
} // namespace tests
} // namespace marvin
#endif
