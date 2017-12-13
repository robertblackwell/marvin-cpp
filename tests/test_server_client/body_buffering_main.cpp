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

#define CATCH_CONFIG_RUNNER
#include "catch.hpp"

#include "rb_logger.hpp"
RBLOGGER_SETLEVEL(LOG_LEVEL_INFO)

#include "http_server.hpp"
#include "request_handler_base.hpp"
#include "bb_client.hpp"
#include "bb_testcase.hpp"
#include "bb_req_handler.hpp"
#include "bb_testcase_defs.hpp"

body_buffering::TestcaseDefinitions tdefs = body_buffering::makeTestcaseDefinitions_01();
body_buffering::TestcaseDefinitions tdefs_eof = body_buffering::makeTCS_eof();
TEST_CASE("body_buffer_01")
{
    body_buffering::Testcase testcase = tdefs_eof.get_case(1);
    boost::asio::io_service io_service;
    body_buffering::TClient tst(io_service, "9991", testcase);
    tst.exec();
    io_service.run();
}
#if 1
TEST_CASE("body_buffer_all")
{
//    body_buffering::Testcase testcase = tdefs.get_case(1);
    for(auto const& testcase: tdefs.cases) {
        boost::asio::io_service io_service;
        body_buffering::TClient tst(io_service, "9991", testcase);
        tst.exec();
        io_service.run();
    }
}
#endif
int main( int argc, char* argv[] )
{
    // global setup - run a server
    RBLogging::setEnabled(false);
    std::thread server_thread([](){
        try {
            HTTPServer server([](boost::asio::io_service& io){
                return new body_buffering::RequestHandler(io);
            });
            server.listen();
        } catch(std::exception & ex) {
            return;
        }
    });

    int result = Catch::Session().run( argc, argv );
    // now wait for the saerver to complete its cleanup after the last request
    sleep(10);

    raise(SIGTERM); // kill server - in xcode debugger this will not stop the server - but works when not in debugger
    server_thread.join();
    return result;
}
