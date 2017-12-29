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

#include "boost_stuff.hpp"
#include "rb_logger.hpp"
RBLOGGER_SETLEVEL(LOG_LEVEL_INFO)

#include "tsc_req_handler.hpp"
#include "server_runner.hpp"

int main( int argc, char* argv[] )
{
    // global setup - run a server
    RBLogging::setEnabled(false);
#define EX_RUNNER
#ifdef EX_RUNNER
    ServerRunner<TscRequestHandler> s_runner;
    s_runner.setup();
#endif
    char* _argv[2] = {argv[0], (char*)"--gtest_filter=*.*"}; // change the filter to restrict the tests that are executed
    int _argc = 2;
    testing::InitGoogleTest(&_argc, _argv);
    auto res = RUN_ALL_TESTS();
#ifdef EX_RUNNER
    sleep(1);
    s_runner.teardown();
#endif
    return res;
}
