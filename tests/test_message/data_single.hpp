#ifndef marvin_tests_message_data_single_hpp
#define marvin_tests_message_data_single_hpp

#include "testcase_defs.hpp"
#include <iostream>
#include <iterator>
#include <algorithm>
#include <vector>
#include <string>
#include <map>

#include "helper_types.hpp"

using namespace Marvin;
using namespace Tests;
using namespace Message;

TestCollection& single_message_test_data_2()
{
    static TestCollection ptc;
    TestSet ts;
    ptc["simple_200"] = TestSet{
        "index 0 - simple 200 body length 10",
        std::vector<char *>{
            (char *) "HTTP/1.1 200 OK 11Reason Phrase\r\n",
            (char *) "Host: ahost\r\n",
            (char *) "Connection: keep-alive\r\n",
            (char *) "Proxy-Connection: keep-alive\r\n",
            (char *) "Content-length: 11\r\n\r\n",
            (char *) "01234567890",
            NULL
        },
        [](std::vector<MessageBase::SPtr> messages)
        {
            MessageBase::SPtr m1 = messages[0];
            HeaderFields& h = m1->headers();
            auto x = m1->status_code();
                CHECK((m1->status_code() == 200));
                CHECK((m1->reason() == "OK 11Reason Phrase"));
                CHECK(h.at_key(HeaderFields::Host));
                CHECK(h.at_key(HeaderFields::Connection));
                CHECK(h.at_key(HeaderFields::ProxyConnection));
                CHECK(h.at_key(HeaderFields::ContentLength));
                CHECK(h.at_key(HeaderFields::Host).get() == "ahost");
                CHECK(h.at_key(HeaderFields::Connection).get() == "keep-alive");
                CHECK(h.at_key(HeaderFields::ProxyConnection).get() == "keep-alive");
                CHECK(h.at_key(HeaderFields::ContentLength).get() == "11");
                CHECK(m1->get_body_buffer_chain()->to_string() == "01234567890");
        }
    };
    ptc["simple_200_emptyline"] = TestSet{
        "index 1 - simple 200 body length 11 no body data in header buffer",
        std::vector<char *>{
            (char *) "HTTP/1.1 200 OK 11Reason Phrase\r\n",
            (char *) "Connection: keep-alive\r\n",
            (char *) "Proxy-Connection: keep-alive\r\n",
            (char *) "Content-length: 11\r\n",
            (char *) "\r\n",
            (char *) "11234567890",
            NULL
        },
        [](std::vector<MessageBase::SPtr> messages)
        {
            MessageBase::SPtr m1 = messages[0];
            HeaderFields& h = m1->headers();
            auto x = m1->status_code();
                CHECK((m1->status_code() == 200));
                CHECK((m1->reason() == "OK 11Reason Phrase"));
                CHECK(!h.at_key(HeaderFields::Host));
                CHECK(h.at_key(HeaderFields::Connection));
                CHECK(h.at_key(HeaderFields::ProxyConnection));
                CHECK(h.at_key(HeaderFields::ContentLength));
                CHECK(h.at_key(HeaderFields::Connection).get() == "keep-alive");
                CHECK(h.at_key(HeaderFields::ProxyConnection).get() == "keep-alive");
                CHECK(h.at_key(HeaderFields::ContentLength).get() == "11");
                CHECK(m1->get_body_buffer_chain()->to_string() == "11234567890");
        }
    };
    ptc["201-body_header_overlay"] = TestSet{
        "index 2 - 201 body length 10 SOME body data in header buffer",
        std::vector<char *>{
            (char *) "HTTP/1.1 201 OK 22Reason Phrase\r\n",
            (char *) "Host: ahost\r\n",
            (char *) "Connection: keep-alive\r\n",
            (char *) "Proxy-Connection: keep-alive\r\n",
            (char *) "Content-length: 10\r\n\r\nAB",
            (char *) "CDEFGHIJ",
            NULL
        },
        [](std::vector<MessageBase::SPtr> messages)
        {
            MessageBase::SPtr m1 = messages[0];
            HeaderFields& h = m1->headers();
            auto x = m1->status_code();
                CHECK((m1->status_code() == 201));
                CHECK((m1->reason() == "OK 22Reason Phrase"));
                CHECK(h.at_key(HeaderFields::Host));
                CHECK(h.at_key(HeaderFields::Connection));
                CHECK(h.at_key(HeaderFields::ProxyConnection));
                CHECK(h.at_key(HeaderFields::ContentLength));
                CHECK(h.at_key(HeaderFields::Host).get() == "ahost");
                CHECK(h.at_key(HeaderFields::Connection).get() == "keep-alive");
                CHECK(h.at_key(HeaderFields::ProxyConnection).get() == "keep-alive");
                CHECK(h.at_key(HeaderFields::ContentLength).get() == "10");
                CHECK(m1->get_body_buffer_chain()->to_string() == "ABCDEFGHIJ");
        }
    };

    // 3 EOH and EOM at the same time
    ptc["201-body_in_blankLine"] = TestSet{
        "index 3 - simple 201 body length 10 SOME body data in with blank line buffer",
        std::vector<char *>{
            (char *) "HTTP/1.1 201 OK 22Reason Phrase\r\n",
            (char *) "Host: ahost\r\n",
            (char *) "Connection: keep-alive\r\n",
            (char *) "Proxy-Connection: keep-alive\r\n",
            (char *) "Content-length: 10",
            (char *) "\r\n\r\nABCDEFGHIJ",
            NULL
        },
        [](std::vector<MessageBase::SPtr> messages)
        {
            MessageBase::SPtr m1 = messages[0];
            HeaderFields& h = m1->headers();
            auto x = m1->status_code();
                CHECK((m1->status_code() == 201));
                CHECK((m1->reason() == "OK 22Reason Phrase"));
                CHECK(h.at_key(HeaderFields::Host));
                CHECK(h.at_key(HeaderFields::Connection));
                CHECK(h.at_key(HeaderFields::ProxyConnection));
                CHECK(h.at_key(HeaderFields::ContentLength));
                CHECK(h.at_key(HeaderFields::Host).get() == "ahost");
                CHECK(h.at_key(HeaderFields::Connection).get() == "keep-alive");
                CHECK(h.at_key(HeaderFields::ProxyConnection).get() == "keep-alive");
                CHECK(h.at_key(HeaderFields::ContentLength).get() == "10");
                CHECK(m1->get_body_buffer_chain()->to_string() == "ABCDEFGHIJ");
        }
    };
    ptc["201-chunked"] = TestSet{
        "index 4 - 201 body chunked encoding NO body data in header buffer",
        std::vector<char *>{
            (char *) "HTTP/1.1 201 OK Reason Phrase\r\n",
            (char *) "Host: ahost\r\n",
            (char *) "Connection: keep-alive\r\n",
            (char *) "Proxy-Connection: keep-alive\r\n",
            (char *) "Transfer-Encoding: chunked\r\n\r\n",
            (char *) "0a\r\n1234567890\r\n",
            (char *) "0f\r\n1234567890XXXXX\r\n",
            (char *) "0a\r\n1234567890\r\n",
            (char *) "0f\r\n1234567890HGHGH\r\n",
            (char *) "0a\r\n1234567890\r\n",
            (char *) "0\r\n",
            (char *) "\r\n",
            NULL
        },
        [](std::vector<MessageBase::SPtr> messages)
        {
            MessageBase::SPtr m1 = messages[0];
            HeaderFields& h = m1->headers();
            auto x = h.at_key(HeaderFields::TransferEncoding);
                CHECK((m1->status_code() == 201));
                CHECK((m1->reason() == "OK Reason Phrase"));
                CHECK(h.at_key(HeaderFields::Host));
                CHECK(h.at_key(HeaderFields::Connection));
                CHECK(h.at_key(HeaderFields::ProxyConnection));
                CHECK(!h.at_key(HeaderFields::ContentLength));
                CHECK(h.at_key(HeaderFields::TransferEncoding));
                CHECK(h.at_key(HeaderFields::Host).get() == "ahost");
                CHECK(h.at_key(HeaderFields::Connection).get() == "keep-alive");
                CHECK(h.at_key(HeaderFields::ProxyConnection).get() == "keep-alive");
                CHECK(h.at_key(HeaderFields::TransferEncoding).get() == "chunked");
                CHECK(m1->get_body_buffer_chain()->to_string()
                      == "12345678901234567890XXXXX12345678901234567890HGHGH1234567890");
        }
    };
    ptc["201-chunked_body in blankline"] = TestSet{
        "index 5 - simple 201 body chunked encoding SOME body data in buffer with blank line after header",
        std::vector<char*> {
            (char *) "HTTP/1.1 201 OK Reason Phrase\r\n",
            (char *) "Host: ahost\r\n",
            (char *) "Connection: keep-alive\r\n",
            (char *) "Proxy-Connection: keep-alive\r\n",
            (char *) "Transfer-Encoding: chunked\r\n",
            (char *) "\r\n0a\r\n1234567890\r\n",
            (char *) "0f\r\n1234567890XXXXX\r\n",
            (char *) "0a\r\n1234567890\r\n",
            (char *) "0f\r\n1234567890HGHGH\r\n",
            (char *) "0a\r\n1234567890\r\n",
            (char *) "0\r\n",
            (char *) "\r\n",
            NULL
        },
        [](std::vector<MessageBase::SPtr> messages)
        {
            MessageBase::SPtr m1 = messages[0];
            HeaderFields& h = m1->headers();
            auto x = h.at_key(HeaderFields::TransferEncoding);
                CHECK((m1->status_code() == 201));
                CHECK((m1->reason() == "OK Reason Phrase"));
                CHECK(h.at_key(HeaderFields::Host));
                CHECK(h.at_key(HeaderFields::Connection));
                CHECK(h.at_key(HeaderFields::ProxyConnection));
                CHECK(!h.at_key(HeaderFields::ContentLength));
                CHECK(h.at_key(HeaderFields::TransferEncoding));
                CHECK(h.at_key(HeaderFields::Host).get() == "ahost");
                CHECK(h.at_key(HeaderFields::Connection).get() == "keep-alive");
                CHECK(h.at_key(HeaderFields::ProxyConnection).get() == "keep-alive");
                CHECK(h.at_key(HeaderFields::TransferEncoding).get() == "chunked");
                CHECK(m1->get_body_buffer_chain()->to_string()
                      == "12345678901234567890XXXXX12345678901234567890HGHGH1234567890");

        }
    };
    ptc["201-chunked_body overlap"] = TestSet{
        "index 6 - simple 201 body chunked encoding SOME body data in buffer with blank line after header",
        std::vector<char *>{
            (char *) "HTTP/1.1 201 OK Reason Phrase\r\n",
            (char *) "Host: ahost\r\n",
            (char *) "Connection: keep-alive\r\n",
            (char *) "Proxy-Connection: keep-alive\r\n",
            (char *) "Transfer-Encoding: chunked\r\n",
            (char *) "\r\n0a\r\n123456",
            (char *) "7890\r\n",
            (char *) "0f\r\n123456",
            (char *) "7890XXXXX\r\n0a\r\n1234567890\r\n",
            (char *) "0f\r\n1234567890HGHGH\r\n",
            (char *) "0a\r\n1234567890\r\n",
            (char *) "0\r\n",
            (char *) "\r\n",
            NULL
        },
        [](std::vector<MessageBase::SPtr> messages)
        {
            MessageBase::SPtr m1 = messages[0];
            HeaderFields& h = m1->headers();
            auto x = h.at_key(HeaderFields::TransferEncoding);
                CHECK((m1->status_code() == 201));
                CHECK((m1->reason() == "OK Reason Phrase"));
                CHECK(h.at_key(HeaderFields::Host));
                CHECK(h.at_key(HeaderFields::Connection));
                CHECK(h.at_key(HeaderFields::ProxyConnection));
                CHECK(!h.at_key(HeaderFields::ContentLength));
                CHECK(h.at_key(HeaderFields::TransferEncoding));
                CHECK(h.at_key(HeaderFields::Host).get() == "ahost");
                CHECK(h.at_key(HeaderFields::Connection).get() == "keep-alive");
                CHECK(h.at_key(HeaderFields::ProxyConnection).get() == "keep-alive");
                CHECK(h.at_key(HeaderFields::TransferEncoding).get() == "chunked");
                CHECK(m1->get_body_buffer_chain()->to_string()
                      == "12345678901234567890XXXXX12345678901234567890HGHGH1234567890");

        }
    };
    ptc["201-chunked_3"] = TestSet{
        "index 7 - simple 201 body chunked encoding SOME body data in buffer with black line after header",
        std::vector<char *>{
            (char *) "HTTP/1.1 201 OK Reason Phrase\r\n",
            (char *) "Host: ahost\r\n",
            (char *) "Connection: keep-alive\r\n",
            (char *) "Proxy-Connection: keep-alive\r\n",
            (char *) "Transfer-Encoding: chunked\r\n",
            (char *) "\r\n",
            (char *) "0a\r\n123456",
            (char *) "7890\r\n",
            (char *) "0f\r\n123456",
            (char *) "7890XXXXX\r\n0a\r\n1234567890\r\n",
            (char *) "0f\r\n1234567890HGHGH\r\n",
            (char *) "0a\r\n1234567890\r\n",
            (char *) "0\r\n",
            (char *) "\r\n",
            NULL
        },
        [](std::vector<MessageBase::SPtr> messages)
        {
            MessageBase::SPtr m1 = messages[0];
            HeaderFields& h = m1->headers();
            auto x = h.at_key(HeaderFields::TransferEncoding);
                CHECK((m1->status_code() == 201));
                CHECK((m1->reason() == "OK Reason Phrase"));
                CHECK(h.at_key(HeaderFields::Host));
                CHECK(h.at_key(HeaderFields::Connection));
                CHECK(h.at_key(HeaderFields::ProxyConnection));
                CHECK(!h.at_key(HeaderFields::ContentLength));
                CHECK(h.at_key(HeaderFields::TransferEncoding));
                CHECK(h.at_key(HeaderFields::Host).get() == "ahost");
                CHECK(h.at_key(HeaderFields::Connection).get() == "keep-alive");
                CHECK(h.at_key(HeaderFields::ProxyConnection).get() == "keep-alive");
                CHECK(h.at_key(HeaderFields::TransferEncoding).get() == "chunked");
                CHECK(m1->get_body_buffer_chain()->to_string()
                      == "12345678901234567890XXXXX12345678901234567890HGHGH1234567890");

        }
    };

    return ptc;
}
#endif