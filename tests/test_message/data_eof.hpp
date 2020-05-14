#ifndef marvin_tests_parser_test_data_tw0_hpp
#define marvin_tests_parser_test_data_two_hpp

#include <iostream>
#include <iterator>
#include <algorithm>
#include <vector>
#include <boost/asio.hpp>
#include <boost/optional.hpp>
#include <boost/optional/optional_io.hpp>
#include <boost/asio/basic_streambuf.hpp>
#include <doctest/doctest.h>
#include <marvin/http/message_base.hpp>
#include <marvin/http/parser.hpp>

#include "helper_types.hpp"

namespace Marvin { 
namespace Tests { 
namespace Message {

TestCollection& test_data_eof()
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
                (char*) "\r\n",
                (char*) "1234567890",
                NULL
        },

        [](std::vector<Marvin::MessageBase*> messages)
        {
            Marvin::MessageBase* m0 = dynamic_cast<Marvin::MessageBase*>(messages[0]);
            CHECK(m0->version_major() == 1);
            CHECK(m0->version_minor() == 1);
            CHECK(m0->status_code() == 200);
            
            HeadersV2& h = m0->headers();

            CHECK(h.atKey("CONNECTION").get() == "keep-alive");
            CHECK(h.atKey("PROXY-CONNECTION").get() == "keep-alive");
            auto b0 = m0->getContentBuffer()->to_string();
            CHECK(m0->getContent()->to_string() == "1234567890");

        }
    };
#if 1
    ptc["200-eof termination_1"] = TestSet{
        "index 0 - terminate with eof(shutdown) - no message length",
        // raw message text
        std::vector<char *>{
            (char *) "HTTP/1.1 200 OK 11Reason Phrase\r\n",
            (char *) "Host: ahost\r\n",
            (char *) "Connection: keep-alive\r\n",
            (char *) "Proxy-Connection: keep-alive\r\n",
            (char *) "\r\n",
            (char *) "01234567890",
            (char *) "eof",
            NULL
        },
        [](std::vector<MessageBase *> messages)
        {
            MessageBase *m1 = messages[0];
            auto x = m1->status_code();
                CHECK((m1->status_code() == 200));
                CHECK((m1->reason() == "OK 11Reason Phrase"));
                HeadersV2& h = m1->headers();
                CHECK( (!!h.atKey(HeadersV2::Host)));
                CHECK( (!!h.atKey(HeadersV2::Connection)));
                CHECK( (!!h.atKey(HeadersV2::ProxyConnection)));
                CHECK(h.atKey(HeadersV2::Host).get() == "ahost");
                CHECK(h.atKey(HeadersV2::Connection).get() == "keep-alive");
                CHECK(h.atKey(HeadersV2::ProxyConnection).get() == "keep-alive");
                CHECK(m1->getContentBuffer()->to_string() == "01234567890");
        }
    };
    ptc["200_close termination"] = TestSet{
        "index 1 - terminate with close - no message length",
        // raw message text
        std::vector<char *>{
            (char *) "HTTP/1.1 200 OK 11Reason Phrase\r\n",
            (char *) "Host: ahost\r\n",
            (char *) "Connection: keep-alive\r\n",
            (char *) "Proxy-Connection: keep-alive\r\n",
            (char *) "\r\n",
            (char *) "01234567890",
            (char *) "close",
            NULL
        },
        [](std::vector<MessageBase *> messages)
        {
            MessageBase *m1 = messages[0];
            auto x = m1->status_code();
                CHECK((m1->status_code() == 200));
                CHECK((m1->reason() == "OK 11Reason Phrase"));
                HeadersV2& h = m1->headers();
                CHECK(h.atKey(HeadersV2::Host));
                CHECK(h.atKey(HeadersV2::Connection));
                CHECK(h.atKey(HeadersV2::ProxyConnection));
                CHECK(h.atKey(HeadersV2::Host).get() == "ahost");
                CHECK(h.atKey(HeadersV2::Connection).get() == "keep-alive");
                CHECK(h.atKey(HeadersV2::ProxyConnection).get() == "keep-alive");
                CHECK(m1->getContentBuffer()->to_string() == "01234567890");
        }
    };

    ptc["200_close_termination_no_body"] = TestSet{
        "index 0 - terminate with eof - no message length and no body",
        // raw message text
        std::vector<char *>{
            (char *) "HTTP/1.1 200 OK 11Reason Phrase\r\n",
            (char *) "Host: ahost\r\n",
            (char *) "Connection: keep-alive\r\n",
            (char *) "Proxy-Connection: keep-alive\r\n",
            (char *) "\r\n",
            (char *) "close",
            NULL
        },
        [](std::vector<MessageBase *> messages)
        {
            MessageBase *m1 = messages[0];
            HeadersV2& h = m1->headers();

            auto x = m1->status_code();
                CHECK((m1->status_code() == 200));
                CHECK((m1->reason() == "OK 11Reason Phrase"));
                CHECK(h.atKey(HeadersV2::Host));
                CHECK(h.atKey(HeadersV2::Connection));
                CHECK(h.atKey(HeadersV2::ProxyConnection));
                CHECK(h.atKey(HeadersV2::Host).get() == "ahost");
                CHECK(h.atKey(HeadersV2::Connection).get() == "keep-alive");
                CHECK(h.atKey(HeadersV2::ProxyConnection).get() == "keep-alive");
                CHECK(m1->getContentBuffer()->to_string() == "");
        }
    };
    ptc["201-hop_header_eof"] = TestSet{
        "index 0 - has hop by hop values in connect header",
        // raw message text
        std::vector<char *>{
            (char *) "HTTP/1.1 200 OK 11Reason Phrase\r\n",
            (char *) "Connection: keep-alive , TE, somethingelse\r\n",
            (char *) "Host: ahost\r\n",
            (char *) "Proxy-Connection: keep-alive\r\n",
            (char *) "\r\n",
            (char *) "01234567890",
            (char *) "eof",
            NULL
        },
        [](std::vector<MessageBase *> messages)
        {
            MessageBase *m1 = messages[0];
            HeadersV2& h = m1->headers();
            auto x = m1->status_code();
                CHECK((m1->status_code() == 200));
                CHECK((m1->reason() == "OK 11Reason Phrase"));
                CHECK(h.atKey(HeadersV2::Host));
                CHECK(h.atKey(HeadersV2::Connection));
                CHECK(h.atKey(HeadersV2::ProxyConnection));
                CHECK(h.atKey(HeadersV2::Host).get() == "ahost");
                CHECK(h.atKey(HeadersV2::Connection).get() == "keep-alive , TE, somethingelse");
                CHECK(h.atKey(HeadersV2::ProxyConnection).get() == "keep-alive");
                CHECK(m1->getContentBuffer()->to_string() == "01234567890");
        }
    };
#endif
    return ptc;
}
} // namespace Parser
} // namespace tests
} // namespace marvin
#endif
