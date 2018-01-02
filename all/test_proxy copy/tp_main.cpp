//
// Start both a server and a number of client tthreads and have each of the client
// threads make a number of requests from the server.
//
#define CATCH_CONFIG_RUNNER
#include <catch/catch.hpp>

#include "rb_logger.hpp"
RBLOGGER_SETLEVEL(LOG_LEVEL_DEBUG)
#include "server_runner.hpp"
#include "tp_proxy_runner.hpp"
TEST_CASE("XP001", "[001]")
{
    REQUIRE(true);
}
TEST_CASE("XP002", "[002]")
{
    REQUIRE(true);
}

int main( int argc, char* argv[] )
{
    RBLogging::setEnabled(false);
    printf("%s\n", __FILE__);
    startTestServer(9992);
//    startProxyServer(9992);
    
    char* _argv[2] = {argv[0], (char*)"--catch_filter=*.*"}; // change the filter to restrict the tests that are executed
    int _argc = 2;
    int result = Catch::Session().run( argc, argv );
    
    stopTestServer();
//    stopProxyServer();
    printf("%s\n", __FILE__);
    return result;
}

