//
// Start both a server and a number of client tthreads and have each of the client
// threads make a number of requests from the server.
//
#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <boost/asio.hpp>
#include <pthread.h>

#define CATCH_CONFIG_RUNNER
#include "catch.hpp"

#include "rb_logger.hpp"
RBLOGGER_SETLEVEL(LOG_LEVEL_INFO)

#include "http_server.hpp"
#include "request_handler_base.hpp"
#include "tsc_post.hpp"
#include "tsc_pipeline.hpp"
//#include "tsc_server.hpp"
#include "tsc_testcase.hpp"
#include "tsc_req_handler.hpp"

int main( int argc, char* argv[] )
{
    // global setup - run a server
    RBLogging::setEnabled(false);
    std::thread server_thread([](){
        try {
            HTTPServer<test_server_client::RequestHandler> server;
            server.listen();
        } catch(std::exception & ex) {
            return;
        }
    });

    int result = Catch::Session().run( argc, argv );

    raise(SIGTERM); // kill server - in xcode debugger this will not stop the server - but works when not in debugger
    server_thread.join();
    return result;
}
