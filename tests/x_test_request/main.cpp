//
// This file makes multiple parallel requests to the same host and checks that
// each reply corresponds to the request. That is ensures no "mixing up".
//
// Note the requests go to a local private host that has been programmed
// specifically for this test
//
// Primarily this is a test of the Request object and uses Request in it's
// simplest for - that is sending an entire http message in one hit and with
// no body.
//
// Consider running this test with different Connection management strategies
// to see whether we can save on "async_connect" calls and maybe even get some
// pipe-lining
//
#include <iostream>
#include <istream>
#include <ostream>
#include <string>
#include <vector>
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include <marvin/boost_stuff.hpp>

#include <marvin/configure_trog.hpp>
TROG_SET_FILE_LEVEL(Trog::LogLevelWarn)
#include <marvin/http/headers_v2.hpp>
#include <marvin/client/client.hpp>
#include <marvin/client/request.hpp>

#undef VERBOSE

TEST_CASE("client_single_roundtrip")
{

}
TEST_CASE("request new")
{
    bool on_headers_flag = false;
    bool on_data_flag;
    bool on_response_flag = false;
    bool on_error_flag = false;

    boost::asio::io_service io_service;

    std::cout << __PRETTY_FUNCTION__  <<std::endl;
    RequestSPtr req;
    if (true) {
        req = std::make_shared<Request>(io_service, "http","localhost","3000");
    } else {
        req = std::make_shared<Request>(io_service, "http","www.whiteacorn.com","80");
    }

#if 1
    req->method(HttpMethod::POST) ;
    req->target("/utests/echo/");
    auto p1 = req->m_current_request->target();

    req->header(Marvin::Http::HeadersV2::ContentType, "text/html; charset=UTF-8");
    req->setOnHeaders([&on_headers_flag](Marvin::ErrorType& err, MessageReaderSPtr msg_sptr) {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
        on_headers_flag = true;
    });
    req->setOnData([&on_data_flag](Marvin::ErrorType err, Marvin::BufferChainSPtr buffer_chain) {
        std::string s =  Marvin::make_error_description(err);
        std::string s2 = buffer_chain->to_string();
        std::cout << __PRETTY_FUNCTION__ << " error : " << s << std::endl;
        std::cout << __PRETTY_FUNCTION__ << " body : " << s2 << std::endl;
        on_data_flag = true;
    });
    req->setOnResponse([&on_response_flag](Marvin::ErrorType err, MessageReaderSPtr msg_sptr) {
        std::string body = msg_sptr->getContent()->to_string();
        std::cout << __PRETTY_FUNCTION__ << " body: " << body <<  std::endl;
        on_response_flag = true;
    });
    req->setOnError([&on_error_flag](Marvin::ErrorType err) {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
        on_error_flag = true;
    });
    std::string body = "This is the body of the request";
    auto p2 = req->m_current_request->str();
    req->asyncWriteLastBodyData(body, [](Marvin::ErrorType& err) {
        std::cout << "Completed the last write" << std::endl;
    });
#endif
    io_service.run();
    CHECK(on_headers_flag);
    CHECK(on_data_flag);
    CHECK(on_response_flag);
    CHECK(!on_error_flag);

}

