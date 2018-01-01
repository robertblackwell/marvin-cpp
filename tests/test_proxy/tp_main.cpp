//
// Start both a server and a number of client tthreads and have each of the client
// threads make a number of requests from the server.
//
#include <gtest/gtest.h>
#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <thread>
#include <pthread.h>
#include "buffer.hpp"
#include "pipe_collector.hpp"
#include "http_server.hpp"
#include "boost_stuff.hpp"
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string/trim.hpp>
#include "rb_logger.hpp"
RBLOGGER_SETLEVEL(LOG_LEVEL_DEBUG)
#include "forwarding_handler.hpp"
#include "forward_helpers.hpp"
#include "tp_handler.hpp"
#include "server_runner.hpp"
#include "tp_proxy_runner.hpp"
#include "runners.hpp"
#if 0
 class XP : public ::testing::Test
 {
    public:
    static ServerRunner destination_server_runner;
        /// start the proxy server with forwarding handler
    static ProxyRunner proxy_server_runner;
    static void SetUpTestCase(){
        std::cout << __FUNCTION__ << std::endl;
        PipeCollector::configSet_PipePath("/Users/rob/marvin_collect");

        std::vector<std::regex> re{std::regex("^ssllabs(.)*$")};
        std::vector<int> ports{443, 9443};
        ForwardingHandler::configSet_HttpsPorts(ports);
        ForwardingHandler::configSet_HttpsHosts(re);

        /// start the endpoint server with our test handler
//        ServerRunner<TscRequestHandler> destination_server_runner;
        /// start the proxy server with forwarding handler
//        ProxyRunner proxy_server_runner;
        destination_server_runner.setup(9991);
        proxy_server_runner.setup(9992);
    }
    static void TearDownTestCase(){
        std::cout << __FUNCTION__ << std::endl;
        sleep(1);
        proxy_server_runner.teardown();
        destination_server_runner.teardown();
    }
    virtual void SetUp(){
        std::cout << __FUNCTION__ << std::endl;
    }
    virtual void TearDown(){
        std::cout << __FUNCTION__ << std::endl;

    }
 };

ServerRunner XP::destination_server_runner;
        /// start the proxy server with forwarding handler
ProxyRunner XP::proxy_server_runner;
#endif
TEST(XP, 001)
{
    EXPECT_TRUE(true);
}
TEST(XP, 002)
{
    EXPECT_TRUE(true);
}
#if 1
int main( int argc, char* argv[] )
{
//    RBLogging::setEnabled(false);
    startTestServer(9991);
    startProxyServer(9992);
    
    char* _argv[2] = {argv[0], (char*)"--gtest_filter=X*.*"}; // change the filter to restrict the tests that are executed
    int _argc = 2;
    testing::InitGoogleTest(&_argc, _argv);
    auto res = RUN_ALL_TESTS();
    
    stopTestServer();
    stopProxyServer();
    return res;
}
#else
int main( int argc, char* argv[] )
{
    // global setup - run a server
//    RBLogging::setEnabled(false);
#define EX_RUNNERxxx
#ifdef EX_RUNNER
    /// config the pipe collector
    PipeCollector::configSet_PipePath("/Users/rob/marvin_collect");

    std::vector<std::regex> re{std::regex("^ssllabs(.)*$")};
    std::vector<int> ports{443, 9443};
    ForwardingHandler::configSet_HttpsPorts(ports);
    ForwardingHandler::configSet_HttpsHosts(re);

    /// start the endpoint server with our test handler
    ServerRunner<TscRequestHandler> destination_server_runner;
    /// start the proxy server with forwarding handler
    ProxyRunner proxy_server_runner;
    destination_server_runner.setup(9991);
    proxy_server_runner.setup(9992);
#endif
    char* _argv[2] = {argv[0], (char*)"--gtest_filter=X*.*"}; // change the filter to restrict the tests that are executed
    int _argc = 2;
    testing::InitGoogleTest(&_argc, _argv);
    auto res = RUN_ALL_TESTS();
#ifdef EX_RUNNER
    sleep(1);
    proxy_server_runner.teardown();
    destination_server_runner.teardown();
#endif
    return res;
}
#endif
