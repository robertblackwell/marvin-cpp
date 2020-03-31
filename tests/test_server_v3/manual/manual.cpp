//
// This file demonstrates how to package tests when when there is a need to start
// a server before the tests start amd stop it at the end
//
#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <thread>
#include <pthread.h>
#define DOCTEST_CONFIG_IMPLEMENT

#include <doctest/doctest.h>

#include <marvin/boost_stuff.hpp>
#include <marvin/external_src/rb_logger/rb_logger.hpp>
RBLOGGER_SETLEVEL(LOG_LEVEL_WARN)
#include "timer.hpp"
#include "v3_handler.hpp"
#include "server_v3_runner.hpp"
#include "any_response.hpp"
#include "../../test_message_roundtrip/runners.hpp"
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


// this is the Marvin server_v3 that was started by main()
std::string scheme = "http";
std::string host = "localhost";
std::string port = "9000";

TEST_CASE("roundtrip_anyresponse")
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

// this makes a roundtrip to a nodejs server that sends a body using chunked encoding
TEST_CASE("roundtripanyresponse_x_3")
{
    std::cout << "round trip by 3" << std::endl; 
    boost::asio::io_service io;
    AnyResponse echo{"/delay/3", HttpMethod::POST, scheme, host, port, "Thisisthebodyoftherequest"};
    RoundTripByN<AnyResponse> runner{io, 10};
    runner.http_exec(echo);
    io.run();
    // sleep(5);
    std::cout << "round trip by 3" << std::endl; 
}
// Run a second time - this will allow us to see if the fd numbers start again. Since
// the server is still active this will give some confidence that we are correctly recycling
// file descriptors associated with sockets.
TEST_CASE("roundtripanyresponse_x_3_v2")
{
    std::cout << "round trip by 3" << std::endl; 
    boost::asio::io_service io;
    AnyResponse echo{"/delay/3", HttpMethod::POST, scheme, host, port, "Thisisthebodyoftherequest"};
    RoundTripByN<AnyResponse> runner{io, 10};
    runner.http_exec(echo);
    io.run();
    std::cout << "round trip by 3" << std::endl; 
}
TEST_CASE("pipeline_x2_roundtrip_anyresponse")
{
    std::cout << "two requests" << std::endl; 
    boost::asio::io_service io;
    AnyResponse echo{"/echo/smart", HttpMethod::POST, scheme, host, port, "Thisisthebodyoftherequest"};
    TwoRequests<AnyResponse> runner{io};
    runner.http_exec(echo);
    io.run();
    std::cout << "two requests" << std::endl; 
}

//
// This is an example of how to run a set of asyn tests that require a server
// be running for those tests
//
int main(int argc, char* argv[])
{
    RBLogging::setEnabled(true);
    Marvin::HttpServer::configSet_NumberOfConnections(5);
    ServerRunner  server_runner;
    server_runner.setup(9000, [](boost::asio::io_service& io)
    {
        return new Handler(io);
    });
    doctest::Context context;
    context.applyCommandLine(argc, argv);
    int result = context.run(); // run
    std::cout << "Final" << std::endl;
    server_runner.terminateServer();
    return result;
}
