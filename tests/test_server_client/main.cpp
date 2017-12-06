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
#include "tsc_client.hpp"
#include "tsc_server.hpp"
#include "tsc_testcase.hpp"

void*
serverFunction(void* arg)
{
    long tid;
    tid = (long)arg;
    printf("Hello World! This is server\n");
    runTestServer();

    return (void*)0;
}
/**
* This function runs the testcases
*/
void* clientFunction(void* arg)
{
    runTestClient();
    return (void*)0;
}
void test()
{
    std::cout.setf(std::ios::unitbuf);
    long retCode;
    void* rcodePtr = &retCode;;
    /**
    * start server
    */
    pthread_t serverThread;
    int s_rc = pthread_create(&serverThread, NULL, serverFunction, (void*)(0));

    pthread_t clientThread;
    int c_rc = pthread_create(&clientThread, NULL, clientFunction, (void*)(0));
    
    pthread_join(clientThread, &rcodePtr);

    return ;
    
//    exit(0); // server will not exit
//    pthread_join(serverThread, &rcodePtr);
//    printf("after joins \r");
}

    EchoTestcase tcase01(
        EchoTestType::WITH_STRING,
        "With string - 2 buffers",
        {
            "1234567890",
            "HGYTRESAWQ"
        }
    );
    EchoTestcase  tcase02(
        EchoTestType::WITH_MBUFFER,
        "With mbuffer - 2 buffers",
        {
            "1234567890",
            "HGYTRESAWQ"
        }
    );
    EchoTestcase tcase03(
        EchoTestType::WITH_BUFFER_CHAIN,
        "With buffer chain - 2 buffers",
        {
            "1234567890",
            "HGYTRESAWQ"
        }
    );

/**
* Test runner
*/
void run_tests()
{
    std::vector<EchoTestcase> tcs = {tcase01, tcase02, tcase03};
    for(auto & c : tcs) {
        test_one<PostTest, EchoTestcase>(c);
    }
    test_multiple<PostTest, EchoTestcase>(tcs);
}
void test_pipeline()
{
    std::vector<EchoTestcase> tcs = {tcase01, tcase02, tcase03};
    test_one<PipelineTest, std::vector<EchoTestcase>>(tcs);
}
/**
* Machinary to get tests started
*/
void cpp_test()
{
    EchoTestcase& tcase = tcase01;
    std::vector<EchoTestcase> tcs = {tcase01, tcase02, tcase03};
    std::thread server_thread([](){
//        auto res = serverFunction( (void*)0);
        runTestServer();
    });
    std::thread client_thread([&tcase](){
        test_pipeline();
    });
    client_thread.join();
    server_thread.join();
    return;
}
int main( int argc, char* argv[] )
{
    // global setup
    
    RBLogging::setEnabled(false);
    int result = Catch::Session().run( argc, argv );

  // global clean-up...

  return result;
//    cpp_test();
}
