//
// Start both a server and a number of client tthreads and have each of the client
// threads make a number of requests from the server.
//
#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <boost/asio.hpp>
#include <thread>
#include <pthread.h>
#include <gtest/gtest.h>

#include "rb_logger.hpp"
RBLOGGER_SETLEVEL(LOG_LEVEL_INFO)

#include "http_server.hpp"
#include "request_handler_base.hpp"
#include "bf_post.hpp"
#include "bf_pipeline.hpp"
#include "bf_testcase.hpp"
#include "bf_req_handler.hpp"

int main( int argc, char* argv[] )
{
    // global setup - run a server
    RBLogging::setEnabled(false);
    std::thread server_thread([](){
        try {
            HTTPServer server([](boost::asio::io_service& io){
                return new body_format::RequestHandler(io);
            });
            server.listen();
        } catch(std::exception & ex) {
            return;
        }
    });
    char* _argv[2] = {argv[0], (char*)"--gtest_filter=*.*"}; // change the filter to restrict the tests that are executed
    int _argc = 2;
    testing::InitGoogleTest(&_argc, _argv);
    auto res = RUN_ALL_TESTS();
    sleep(10);
    raise(SIGTERM); // kill server - in xcode debugger this will not stop the server - but works when not in debugger
    return res;
//    int result = Catch::Session().run( argc, argv );
//    sleep(10);
//    raise(SIGTERM); // kill server - in xcode debugger this will not stop the server - but works when not in debugger
//    server_thread.join();
//    return result;
}
