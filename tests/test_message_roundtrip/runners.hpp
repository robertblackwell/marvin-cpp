#ifndef guard_marvin_test_roundtrip_runners_hpp
#define guard_marvin_test_roundtrip_runners_hpp

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

#include <marvin/boost_stuff.hpp>

#include <marvin/external_src/rb_logger/rb_logger.hpp>
#include <marvin/http/headers_v2.hpp>
#include <marvin/http/message_base.hpp>
#include <marvin/client/client.hpp>

// #include "tm_roundtrip_testcase_base.hpp"
// #include "echo_smart.hpp"
// #include "chunked.hpp"

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
    void operator()(T& testcase){http_exec(testcase);};
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
        request_sptr->setHeader(Marvin::Http::HeadersV2::Connection,"Keep-Alive");
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

template <class T>
class RoundTripByN 
{
public:
    RoundTripByN(boost::asio::io_service& io, int number_of_instances)
    : m_io(io), m_number_of_instances(number_of_instances)
    {

    }
    void http_exec(T& testcase)
    {
        int number_instances = m_number_of_instances;
        for( int i = 0; i < number_instances; i++) {
            RoundTripRunner<T> runner1{m_io};
            m_runners.push_back(runner1); 
        }
        for( int i = 0; i < number_instances; i++) {
            m_runners[i].http_exec(testcase); 
        }
        // RoundTripRunner<T> runner1{m_io};
        // m_runners.push_back(runner1); 
        // RoundTripRunner<T> runner2{m_io}; 
        // m_runners.push_back(runner2); 
        // RoundTripRunner<T> runner3{m_io};
        // m_runners.push_back(runner3); 

        // runner1.exec(testcase);
        // runner2.exec(testcase);
        // runner3.exec(testcase);
    }
    int m_number_of_instances;
    std::vector<RoundTripRunner<T>> m_runners;
    boost::asio::io_service& m_io;
};

#endif