//
// Start both a server and a number of client tthreads and have each of the client
// threads make a number of requests from the server.
//
#include <thread>
#include <boost/process.hpp>
#include <marvin/server/http_server.hpp>
#include <marvin/collector/pipe_collector.hpp>
#include <marvin/forwarding/forwarding_handler.hpp>

#define DOCTEST_CONFIG_IMPLEMENT
#include <doctest/doctest.h>

#include <marvin/collector/collector_base.hpp>

#include "../test_proxy/tp_testcase.hpp"
#include "proxy_fixture.hpp"


int main( int argc, char* argv[] )
{

    ProxyFixture fixture;
    boost::filesystem::path p{__FILE__};

    boost::process::system("/bin/rm", fixture.m_collector_file_path);
    boost::process::system("/usr/bin/touch", fixture.m_collector_file_path);

    std::ofstream outfile(fixture.m_collector_file_path);

    std::vector<std::regex> re{std::regex("^ssllabs(.)*$")};
    std::vector<int> ports{443, 9443};
    ForwardingHandler::configSet_HttpsPorts(ports);
    ForwardingHandler::configSet_HttpsHosts(re);
    HTTPServer* server_ptr;
    auto proxy_func = [&server_ptr, &outfile](void* param) {
        server_ptr = new HTTPServer([&outfile](boost::asio::io_service& io) {
            CollectorBase* collector = new CollectorBase(io, outfile);
            auto f = new ForwardingHandler(io, collector);
            return f;
        });
        server_ptr->listen(9992);
    };
    std::thread proxy_thread(proxy_func, nullptr);


    char* tmp_argv[2] = {argv[0], (char*)"--catch_filter=*.*"}; // change the filter to restrict the tests that are executed
    doctest::Context context;
    context.applyCommandLine(argc, argv);
    int result = context.run(); // run

    server_ptr->terminate();
    proxy_thread.join();

    return result;
}

