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

#include <marvin/external_src/rb_logger/rb_logger.hpp>
RBLOGGER_SETLEVEL(LOG_LEVEL_WARN)
#include <marvin/http/headers_v2.hpp>
#include <marvin/http/message_base.hpp>
#include <marvin/client/client.hpp>

#include "tm_roundtrip_testcase_base.hpp"
#include "echo_smart.hpp"
#include "chunked.hpp"
#include "chunked_error.hpp"
#include "any_response.hpp"
#include "runners.hpp"

std::string port = "3000";
std::string scheme = "http";
std::string host = "localhost";

template <class T, class R>
class ATest
{
public:
    ATest(std::string url, HttpMethod method, std::string scheme, std::string host, std::string port, std::string body)
    {
        boost::asio::io_service m_io;
        T tc(url, method, scheme, host, port, body);
        R runner{m_io};
        runner.http_exec(tc);
        m_io.run();
    }
};

#define MESSAGE_ROUNDTRIP_DISABLE
#ifndef MESSAGE_ROUNDTRIP_DISABLE
// make a round trip to a local nodejs server which echos back the request headers and body in a json response
// the verify parses the json and tests a varierty of header fields
TEST_CASE("roundtrip_test")
{
    std::cout << "first" << std::endl; 
    ATest<AnyResponse, RoundTripRunner<AnyResponse>>(
        "/echo/smart", 
        HttpMethod::POST, 
        scheme, host, 
        port, 
        "This is a body");
    std::cout << "first" << std::endl; 
}
// make a round trip to a local nodejs server which echos back the request headers and body in a json response
// the verify parses the json and tests a varierty of header fields
TEST_CASE("rt_localhost_3000")
{
    std::cout << "round trip test case" << std::endl; 
    boost::asio::io_service io;
    EchoSmart echo{"/echo/smart", HttpMethod::POST, scheme, host, port, "Thisisthebodyoftherequest"};
    RoundTripRunner<EchoSmart> runner{io};
    runner.http_exec(echo);
    io.run();
    std::cout << "round trip test case" << std::endl; 
}
#endif
#ifndef MESSAGE_ROUNDTRIP_DISABLE
// this makes a roundtrip to a nodejs server that sends a body using chunked encoding
TEST_CASE("chunked error during reply")
{
    std::cout << "round trip test case" << std::endl; 
    boost::asio::io_service io;
    ChunkedError chunked{"/chunked_error", HttpMethod::GET, scheme, host, port, ""};
    RoundTripRunner<ChunkedError> runner{io};
    runner(chunked);
    io.run();
    std::cout << "round trip test case" << std::endl; 
}
TEST_CASE("rt_localhost_3000_cnhunked")
{
    std::cout << "round trip test case" << std::endl; 
    boost::asio::io_service io;
    Chunked chunked{"/chunked", HttpMethod::GET, scheme, host, port, ""};
    RoundTripRunner<Chunked> runner{io};
    runner(chunked);
    io.run();
    std::cout << "round trip test case" << std::endl; 
}
// this makes a roundtrip to a nodejs server that sends a body using chunked encoding
TEST_CASE("roundtrip by 3")
{
    std::cout << "round trip by 3" << std::endl; 
    boost::asio::io_service io;
    EchoSmart echo{"/echo/smart", HttpMethod::POST, scheme, host, port, "Thisisthebodyoftherequest"};
    RoundTripByN<EchoSmart> runner{io, 10};
    runner.http_exec(echo);
    io.run();
    std::cout << "round trip by 3" << std::endl; 
}
// this makes a roundtrip to a nodejs server that sends a body using chunked encoding
TEST_CASE("any response by 3")
{
    std::cout << "round trip by 3" << std::endl; 
    boost::asio::io_service io;
    AnyResponse tc{"/echo/smart", HttpMethod::POST, scheme, host, port, "Thisisthebodyoftherequest"};
    RoundTripByN<AnyResponse> runner{io, 10};
    runner.http_exec(tc);
    io.run();
    std::cout << "any response by 3" << std::endl; 
}
#endif

#ifndef MESSAGE_ROUNDTRIP_DISABLE

TEST_CASE("rt_localhost_3000_explicit_connect_smart_echo")
{
    std::cout << "explicit connect smart echo" << std::endl; 
    boost::asio::io_service io;
    EchoSmart echo{"/echo/smart", HttpMethod::POST, scheme, host, port, "Thisisthebodyoftherequest"};
    ExplicitConnect<EchoSmart> runner{io};
    runner.http_exec(echo);
    io.run();
    std::cout << "round trip test case" << std::endl; 
}


TEST_CASE("two smart echo")
{
    std::cout << "two requests" << std::endl; 
    boost::asio::io_service io;
    EchoSmart echo{"/echo/smart", HttpMethod::POST, scheme, host, port, "Thisisthebodyoftherequest"};
    TwoRequests<EchoSmart> runner{io};
    runner.http_exec(echo);
    io.run();
    std::cout << "two requests" << std::endl; 
}
TEST_CASE("two chunked")
{
    std::cout << "two requests" << std::endl; 
    boost::asio::io_service io;
    Chunked chunked{"/echo/smart", HttpMethod::POST, scheme, host, port, "Thisisthebodyoftherequest"};
    TwoRequests<Chunked> runner{io};
    runner.http_exec(chunked);
    io.run();
    std::cout << "two requests" << std::endl; 
}
#endif
TEST_CASE("N chunked")
{
    std::cout << "N requests" << std::endl; 
    boost::asio::io_service io;
    Chunked chunked{"/echo/smart", HttpMethod::POST, scheme, host, port, "Thisisthebodyoftherequest"};
    NBackToBackRequests<Chunked> runner{io, chunked, 3};
    runner.http_exec();
    io.run();
    std::cout << "N requests" << std::endl; 
}
#if 0 

TEST_CASE("whiteacorn.com_utests_echo/")
{
    std::cout << "round trip test case" << std::endl; 
    boost::asio::io_service io;
    EchoSmart echo{"/utest/echo/", HttpMethod::POST, scheme, "whiteacorn.com", "80", "Thisisthebodyoftherequest"};
    RoundTripRunner<EchoSmart> runner{io};
    runner.http_exec(echo);
    io.run();
    std::cout << "round trip test case" << std::endl; 
}
#endif