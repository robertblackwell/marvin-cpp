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
#include <marvin/external_src/rb_logger/rb_logger.hpp>
RBLOGGER_SETLEVEL(LOG_LEVEL_WARN)

#include "v2_handler.hpp"
#include "server_v2_runner.hpp"

int main(int argc, char** argv) {
    doctest::Context context;

    RBLogging::setEnabled(false);
    ServerRunnerV2 s_runner;
    s_runner.setup(9000);

    context.addFilter("test-case-exclude", "*math*"); // exclude test cases with "math" in their name
    context.setOption("abort-after", 5);              // stop test execution after 5 failed assertions
    context.setOption("order-by", "name");            // sort the test cases by their name
    argc = 1;
    context.applyCommandLine(argc, argv);
    context.setOption("no-breaks", true);             // don't break in the debugger when assertions fail
    int res = context.run(); // run
#if 0
    sleep(2);
    boost::asio::io_service io;
    ConnectionSPtr conn_sptr = std::make_shared<Connection>(io, "http", "localhost", "9000");
    conn_sptr->asyncConnect([conn_sptr](Marvin::ErrorType& err, ISocket* sock){
        std::string req = "GET /echo/smart HTTP/1.1\r\nContent-type: text/plain; charset=utf-8;Content-length: 0\r\n\r\n";
        std::cout << __PRETTY_FUNCTION__ << std::endl;
        conn_sptr->asyncWrite(req, [conn_sptr](Marvin::ErrorType& err, std::size_t bytes){
            // conn_sptr->rea
            std::cout << __PRETTY_FUNCTION__ << std::endl;
        });
    });
#endif
    sleep(100);
    s_runner.teardown();
    
    return res;
}
