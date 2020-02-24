//
// Start both a server and a number of client tthreads and have each of the client
// threads make a number of requests from the server.
//
#include <thread>
#include <boost/process.hpp>
#include "http_server.hpp"
#include "pipe_collector.hpp"
#include "forwarding_handler.hpp"
#define CATCH_CONFIG_RUNNER
#include <catch2/catch.hpp>

#include "rb_logger.hpp"
RBLOGGER_SETLEVEL(LOG_LEVEL_DEBUG)

#include "../test_proxy/tp_testcase.hpp"
#include "../test_proxy/tp_proxy_tests.hpp"

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
    HTTPServer* server_ptr;
    auto proxy_func = [&server_ptr](void* param) {
        server_ptr = new HTTPServer([](boost::asio::io_service& io){
            std::cout << "lambda creating HttpServer" << std::endl;
            PipeCollector* pc = PipeCollector::getInstance(io);
            std::cout << "lambda creating HttpServer after pipe collector getInstance" << std::endl;
            auto f = new ForwardingHandler(io, pc);
            std::cout << "lambda creating HttpServer after create forwarding handler" << std::endl;
            return f;
        });
        server_ptr->listen(9992);
    };
    std::thread proxy_thread(proxy_func, nullptr);
    
    auto catch_func = [argv, argc](void* param) {
        char* _argv[2] = {argv[0], (char*)"--catch_filter=*.*"}; // change the filter to restrict the tests that are executed
        int _argc = 2;
        int result = Catch::Session().run( argc, argv );
    };

    std::thread catch_thread(catch_func, nullptr);

    catch_thread.join();
    server_ptr->terminate();
    proxy_thread.join();
}

