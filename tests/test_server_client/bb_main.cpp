//
// Start both a server and a number of client tthreads and have each of the client
// threads make a number of requests from the server.
//
#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <marvin/boost_stuff.hpp>
#include <thread>
#include <pthread.h>
#define DOCTEST_CONFIG_IMPLEMENT
#include <doctest/doctest.h>

#include <marvin/external_src/rb_logger/rb_logger.hpp>
RBLOGGER_SETLEVEL(LOG_LEVEL_INFO)

#include <marvin/server/http_server.hpp>
#include <marvin/server/request_handler_base.hpp>

#include "tsc_req_handler.hpp"
#include "server_runner.hpp"
#include "bb_client.hpp"
#include "bb_testcase.hpp"
#include "bb_testcase_defs.hpp"


int main( int argc, char* argv[] )
{
    // global setup - run a server
    RBLogging::setEnabled(false);
    /// limit the number of simultaneous connections
    /// this set of tests will exercise connection wait logic
    HTTPServer::configSet_NumberOfConnections(2);

    doctest::Context context;
    context.applyCommandLine(argc, argv);
    int result = context.run(); // run
    
    stopTestServer();
    printf("%s\n",__FILE__);
    return result;
}
