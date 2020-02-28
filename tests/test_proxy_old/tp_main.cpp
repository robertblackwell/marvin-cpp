//
// Start both a server and a number of client tthreads and have each of the client
// threads make a number of requests from the server.
//
#include <thread>
#include <boost/process.hpp>
#define CATCH_CONFIG_RUNNER
#include <catch2/catch.hpp>

#include <marvin/external_src/rb_logger/rb_logger.hpp>
RBLOGGER_SETLEVEL(LOG_LEVEL_DEBUG)
#include <marvin/collector/collector_base.hpp>


int main( int argc, char* argv[] )
{
    LogTrace("hello");
    VLogDebug("hello");
    std::cout << "in main" << std::endl;
    RBLogging::enableForLevel(LOG_LEVEL_WARN);
    boost::filesystem::path p{__FILE__};
    boost::filesystem::path d = p.parent_path();
    boost::filesystem::path c = d / "whiteacorn_received";
    boost::filesystem::path e = d / "whiteacorn_expected";
    boost::filesystem::path f = d / "whiteacorn_received_fixed";
    std::string collector_file_path = c.string();

    boost::process::system("/bin/rm", collector_file_path);
    boost::process::system("/usr/bin/touch", collector_file_path);

    PipeCollector::configSet_PipePath(collector_file_path);
    std::vector<std::regex> re{std::regex("^ssllabs(.)*$")};
    std::vector<int> ports{443, 9443};
    ForwardingHandler::configSet_HttpsPorts(ports);
    ForwardingHandler::configSet_HttpsHosts(re);
    ProxyRunnerSPtr proxyRunnerSPtr = std::make_shared<ProxyRunner>();
    proxyRunnerSPtr->setup(9992);
    std::thread* t = proxyRunnerSPtr->server_thread_ptr;
    char* _argv[2] = {argv[0], (char*)"--catch_filter=*.*"}; // change the filter to restrict the tests that are executed
    int _argc = 2;
    int result = Catch::Session().run( argc, argv );
    t->join();
    stopTestServer();
    stopProxyServer();
    return result;
}

