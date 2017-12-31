//
// Start both a server and a number of client tthreads and have each of the client
// threads make a number of requests from the server.
//
#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <thread>
#include <pthread.h>
#include <gtest/gtest.h>
#include "buffer.hpp"
#include "pipe_collector.hpp"
#include "http_server.hpp"
#include "boost_stuff.hpp"
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string/trim.hpp>
#include "rb_logger.hpp"
RBLOGGER_SETLEVEL(LOG_LEVEL_DEBUG)
#include "forward_helpers.hpp"
#include "tp_handler.hpp"
#include "tp_server_runner.hpp"


int main( int argc, char* argv[] )
{
    // global setup - run a server
//    RBLogging::setEnabled(false);
#define EX_RUNNER
#ifdef EX_RUNNER__X
    /// config the pipe collector
    PipeCollector::configSet_PipePath("/Users/rob/marvin_collect");

    std::vector<std::regex> re{std::regex("^ssllabs(.)*$")};
    std::vector<int> ports{443, 9443};
    ForwardingHandler<PipeCollector>::configSet_HttpsPorts(ports);
    ForwardingHandler<PipeCollector>::configSet_HttpsHosts(re);
    using ForwardHandler = ForwardingHandler<PipeCollector>;

    /// start the endpoint server with our test handler
    ServerRunner<TscRequestHandler> destination_server_runner;
    /// start the proxy server with forwarding handler
    ServerRunner<ForwardHandler> proxy_server_runner;
    destination_server_runner.setup(9991);
    proxy_server_runner.setup(9992);
#endif
    char* _argv[2] = {argv[0], (char*)"--gtest_filter=*.*"}; // change the filter to restrict the tests that are executed
    int _argc = 2;
    testing::InitGoogleTest(&_argc, _argv);
    auto res = RUN_ALL_TESTS();
#ifdef EX_RUNNER__X
    sleep(1);
    proxy_server_runner.teardown();
    destination_server_runner.teardown();
#endif
    return res;
}
