//
// Start both a server and a number of client tthreads and have each of the client
// threads make a number of requests from the server.
//
#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <marvin/include/boost_stuff.hpp>
#include <thread>
#include <pthread.h>
#define CATCH_CONFIG_RUNNER
#include <catch2/catch.hpp>

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
    startTestServer();
    char* _argv[2] = {argv[0], (char*)""}; // change the filter to restrict the tests that are executed
//    char* _argv[2] = {argv[0], (char*)"[tc]"}; // change the filter to restrict the tests that are executed
//    char* _argv[2] = {argv[0], (char*)"[parallelall]"}; // change the filter to restrict the tests that are executed
//    char* _argv[2] = {argv[0], (char*)"[parallel2]"}; // change the filter to restrict the tests that are executed
//    char* _argv[2] = {argv[0], (char*)"[to]"}; // change the filter to restrict the tests that are executed
    int _argc = 2;
    printf("%s\n",__FILE__);
    int result = Catch::Session().run( argc, argv );
    stopTestServer();
    printf("%s\n",__FILE__);
    return result;
}
