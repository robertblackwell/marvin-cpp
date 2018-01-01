//
// Start both a server and a number of client tthreads and have each of the client
// threads make a number of requests from the server.
//
#define CATCH_CONFIG_RUNNER
#include <catch/catch.hpp>

#include "rb_logger.hpp"
RBLOGGER_SETLEVEL(LOG_LEVEL_DEBUG)
#include "runners.hpp"

TEST_CASE("XP001", "[001]")
{
    REQUIRE(true);
}
TEST_CASE("XP002", "[002]")
{
    REQUIRE(true);
}
#if 1
int main( int argc, char* argv[] )
{
    RBLogging::setEnabled(false);
    startTestServer(9991);
    startProxyServer(9992);
    
    char* _argv[2] = {argv[0], (char*)"--catch_filter=X*.*"}; // change the filter to restrict the tests that are executed
    int _argc = 2;
    int result = Catch::Session().run( argc, argv );
    
    stopTestServer();
    stopProxyServer();
    return result;
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
    char* _argv[2] = {argv[0], (char*)"--catch_filter=X*.*"}; // change the filter to restrict the tests that are executed
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
