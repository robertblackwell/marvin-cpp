#ifndef marvin_tests_parser_test_data_tw0_hpp
#define marvin_tests_parser_test_data_two_hpp

#include <iostream>
#include <iterator>
#include <algorithm>
#include <vector>
#include <boost/asio.hpp>
#include <boost/asio/basic_streambuf.hpp>
#include <doctest/doctest.h>
#include <marvin/http/message_base.hpp>
#include <marvin/http/parser.hpp>

#include "helper_types.hpp"

namespace Marvin { 
namespace Tests { 
namespace Message {

TestCollection& test_data_two_message()
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
            HeadersV2& h = m0->headers();

            REQUIRE(m0 != nullptr);
            CHECK(m0->version_major() == 1);
            CHECK(m0->version_minor() == 1);
            CHECK(m0->status_code() == 200);
            CHECK(h.atKey("CONTENT-LENGTH").get() == "10");
            CHECK(h.atKey("CONNECTION").get() == "keep-alive");
            CHECK(h.atKey("PROXY-CONNECTION").get() == "keep-alive");
            auto b0 = m0->getContentBuffer()->to_string();
            CHECK(m0->getContent()->to_string() == "1234567890");

            Marvin::MessageBase* m1 = dynamic_cast<Marvin::MessageBase*>(messages[1]);
            REQUIRE(m1 != nullptr);
            h = m1->headers();
            CHECK(m1->version_major() == 1);
            CHECK(m1->version_minor() == 1);
            CHECK(m1->status_code() == 201);
            CHECK(h.atKey("CONTENT-LENGTH").get() == "11");
            CHECK(h.atKey("CONNECTION").get() == "keep-alive");
            CHECK(h.atKey("PROXY-CONNECTION").get() == "keep-alive");

            auto b1 = m1->getContentBuffer()->to_string();
            CHECK(m1->getContent()->to_string() == "ABCDEFGHIJK");
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
            HeadersV2& h = msg_p->headers();
            CHECK(msg_p->version_major() == 1);
            CHECK(msg_p->version_minor() == 1);
            CHECK(msg_p->method_string() == "GET");
            CHECK(h.atKey("CONTENT-LENGTH").get() == "10");
            CHECK(h.atKey("CONNECTION").get() == "keep-alive");
            CHECK(h.atKey("PROXY-CONNECTION").get() == "keep-alive");
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
            HeadersV2& h = msg_p->headers();

            CHECK(msg_p->version_major() == 1);
            CHECK(msg_p->version_minor() == 1);
            CHECK(msg_p->status_code() == 201);
            CHECK(! h.atKey("CONTENT-LENGTH"));
            CHECK(h.atKey("TRANSFER-ENCODING").get() == "chunked");
            CHECK(h.atKey("CONNECTION").get() == "keep-alive");
            CHECK(h.atKey("PROXY-CONNECTION").get() == "keep-alive");
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
            HeadersV2& h = msg_p->headers();
            CHECK(msg_p->version_major() == 1);
            CHECK(msg_p->version_minor() == 1);
            CHECK(msg_p->status_code() == 201);
            CHECK(! h.atKey("CONTENT-LENGTH"));
            CHECK(h.atKey("TRANSFER-ENCODING").get() == "chunked");
            CHECK(h.atKey("CONNECTION").get() == "keep-alive");
            CHECK(h.atKey("PROXY-CONNECTION").get() == "keep-alive");
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
            HeadersV2& h = msg_p->headers();
            CHECK(msg_p->version_major() == 1);
            CHECK(msg_p->version_minor() == 1);
            CHECK(msg_p->status_code() == 201);
            CHECK(! h.atKey("CONTENT-LENGTH"));
            CHECK(h.atKey("TRANSFER-ENCODING").get() == "chunked");
            CHECK(h.atKey("CONNECTION").get() == "keep-alive");
            CHECK(h.atKey("PROXY-CONNECTION").get() == "keep-alive");
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
