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
#define DOCTEST_CONFIG_IMPLEMENT
#include <doctest/doctest.h>

#include <marvin/boost_stuff.hpp>
#include <marvin/external_src/trog/trog.hpp>
Trog_SETLEVEL(LOG_LEVEL_WARN)

#include "server_runner.hpp"
//#include "bf_fix_multiple.hpp"

#if 0
int main( int argc, char* argv[] )
{
    // global setup - run a server
    RBLogging::setEnabled(false);
    char* _argv[2] = {argv[0], (char*)"--catch_filter=*.*"}; // change the filter to restrict the tests that are executed
    int _argc = 2;
    int result = Catch::Session().run( argc, argv );
    return result;
}

#else
int main( int argc, char* argv[] )
{
    // global setup - run a server
    RBLogging::setEnabled(false);
#define EX_RUNNER
#ifdef EX_RUNNER
    ServerRunner s_runner;
    s_runner.setup(9900);
#endif
    int result;
    // doctest::Context context;
    // context.applyCommandLine(argc, argv);
    // result = context.run(); // run
#ifdef EX_RUNNER
    sleep(100);
    s_runner.teardown();
#endif
    printf("%s\n",__FILE__);
    return result;
}
#endif
