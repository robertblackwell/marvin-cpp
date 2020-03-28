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
#include <marvin/http/http_header.hpp>
#include <marvin/http/message_base.hpp>
#include <marvin/client/client.hpp>
#include "tm_roundtrip_testcase_base.hpp"
#include "echo_smart.hpp"
#include "chunked.hpp"

template <class T>
class RoundTripRunner {
public:
    RoundTripRunner(boost::asio::io_service& io): m_io(io){}
    void http_exec(T& testcase)
    {
        using namespace Marvin::Http;
        using namespace Marvin;
        MessageBaseSPtr request_sptr = testcase.makeRequest();
        Marvin::BufferChainSPtr body = testcase.makeBody();
        m_host = testcase.getHost();
        m_port = testcase.getPort();
        std::cout << __PRETTY_FUNCTION__ << std::endl;
        m_client_sptr = std::shared_ptr<Client>(new Client(m_io, "http", m_host, m_port));
        m_client_sptr->asyncWrite(request_sptr, body, [this, &testcase](ErrorType& err,  MessageBaseSPtr response_sptr) {
            testcase.verifyResponse(err, response_sptr);
        });
    }

    void https_exec(std::string host, std::string port){};
    std::string m_scheme;
    std::string m_host;
    std::string m_port;
    boost::asio::io_service& m_io;
    ClientSPtr  m_client_sptr;

};
template <class T>
class ExplicitConnect {
public:
    ExplicitConnect(boost::asio::io_service& io): m_io(io){}
    void http_exec(T& testcase)
    {
        using namespace Marvin::Http;
        using namespace Marvin;
        MessageBaseSPtr request_sptr = testcase.makeRequest();
        Marvin::BufferChainSPtr body = testcase.makeBody();
        m_host = testcase.getHost();
        m_port = testcase.getPort();
        std::cout << __PRETTY_FUNCTION__ << std::endl;
        m_client_sptr = std::shared_ptr<Client>(new Client(m_io, "http", m_host, m_port));
        m_client_sptr->asyncConnect([this, &testcase, request_sptr, body](Marvin::ErrorType err){
            m_client_sptr->asyncWrite(request_sptr, body, [this, &testcase](ErrorType& err,  MessageBaseSPtr response_sptr) {
                testcase.verifyResponse(err, response_sptr);
            });
        });
    }

    void https_exec(std::string host, std::string port){};
    std::string m_scheme;
    std::string m_host;
    std::string m_port;
    boost::asio::io_service& m_io;
    ClientSPtr  m_client_sptr;
};
// two requests on the same connection
template <class T>
class TwoRequests {
public:
    TwoRequests(boost::asio::io_service& io): m_io(io){}
    void http_exec(T& testcase)
    {
        using namespace Marvin::Http;
        using namespace Marvin;
        MessageBaseSPtr request_sptr = testcase.makeRequest();
        Marvin::BufferChainSPtr body = testcase.makeBody();
        m_host = testcase.getHost();
        m_port = testcase.getPort();
        std::cout << __PRETTY_FUNCTION__ << std::endl;
        m_client_sptr = std::shared_ptr<Client>(new Client(m_io, "http", m_host, m_port));
        m_client_sptr->asyncConnect([this, &testcase, request_sptr, body](Marvin::ErrorType err){
            m_client_sptr->asyncWrite(request_sptr, body, [this, &testcase](ErrorType& err,  MessageBaseSPtr response_sptr) {
                testcase.verifyResponse(err, response_sptr);
                MessageBaseSPtr another_request_sptr = testcase.makeRequest();
                Marvin::BufferChainSPtr another_body = testcase.makeBody();
                m_client_sptr->asyncWrite(another_request_sptr, another_body, [this, &testcase](ErrorType& err,  MessageBaseSPtr another_response_sptr) {
                    REQUIRE(!err);
                    testcase.verifyResponse(err, another_response_sptr);
                });
            });
        });
    }

    void https_exec(std::string host, std::string port){};
    std::string m_scheme;
    std::string m_host;
    std::string m_port;
    boost::asio::io_service& m_io;
    ClientSPtr  m_client_sptr;
};


#if 0
// make a round trip to a local nodejs server which echos back the request headers and body in a json response
// the verify parses the json and tests a varierty of header fields
TEST_CASE("rt_localhost_3000")
{
    std::cout << "round trip test case" << std::endl; 
    boost::asio::io_service io;
    EchoSmart echo{"/echo/smart", HttpMethod::POST, "http", "localhost", "3000", "Thisisthebodyoftherequest"};
    RoundTripRunner<EchoSmart> runner{io};
    runner.http_exec(echo);
    io.run();
    std::cout << "round trip test case" << std::endl; 
}
TEST_CASE("rt_localhost_3000_cnhunked")
{
    std::cout << "round trip test case" << std::endl; 
    boost::asio::io_service io;
    Chunked chunked{"/chunked", HttpMethod::GET, "http", "localhost", "3000", ""};
    RoundTripRunner<Chunked> runner{io};
    runner.http_exec(chunked);
    io.run();
    std::cout << "round trip test case" << std::endl; 
}
#endif
// this makes a roundtrip to a nodejs server that sends a body using chunked encoding
TEST_CASE("chunked error during reply")
{
    std::cout << "round trip test case" << std::endl; 
    boost::asio::io_service io;
    Chunked chunked{"/chunked_error", HttpMethod::GET, "http", "localhost", "3000", ""};
    RoundTripRunner<Chunked> runner{io};
    runner.http_exec(chunked);
    io.run();
    std::cout << "round trip test case" << std::endl; 
}
#if 0
TEST_CASE("rt_localhost_3000_explicit_connect_smart_echo")
{
    std::cout << "explicit connect smart echo" << std::endl; 
    boost::asio::io_service io;
    EchoSmart echo{"/echo/smart", HttpMethod::POST, "http", "localhost", "3000", "Thisisthebodyoftherequest"};
    ExplicitConnect<EchoSmart> runner{io};
    runner.http_exec(echo);
    io.run();
    std::cout << "round trip test case" << std::endl; 
}


TEST_CASE("two smart echo")
{
    std::cout << "two requests" << std::endl; 
    boost::asio::io_service io;
    EchoSmart echo{"/echo/smart", HttpMethod::POST, "http", "localhost", "3000", "Thisisthebodyoftherequest"};
    TwoRequests<EchoSmart> runner{io};
    runner.http_exec(echo);
    io.run();
    std::cout << "two requests" << std::endl; 
}
TEST_CASE("two chunked")
{
    std::cout << "two requests" << std::endl; 
    boost::asio::io_service io;
    Chunked chunked{"/echo/smart", HttpMethod::POST, "http", "localhost", "3000", "Thisisthebodyoftherequest"};
    TwoRequests<Chunked> runner{io};
    runner.http_exec(chunked);
    io.run();
    std::cout << "two requests" << std::endl; 
}
#endif
#if 0
TEST_CASE("whiteacorn.com_utests_echo/")
{
    std::cout << "round trip test case" << std::endl; 
    boost::asio::io_service io;
    EchoSmart echo{"/utest/echo/", "http", "whiteacorn.com", "80", "Thisisthebodyoftherequest"};
    RoundTripRunner<EchoSmart> runner{io};
    runner.http_exec(echo);
    io.run();
    std::cout << "round trip test case" << std::endl; 
}
#endif