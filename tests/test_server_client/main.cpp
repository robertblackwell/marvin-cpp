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
#include <gtest/gtest.h>"
#include "rb_logger.hpp"
RBLOGGER_SETLEVEL(LOG_LEVEL_INFO)
#include "http_server.hpp"
#include "request_handler_base.hpp"
#include "tsc_client.hpp"
#include "tsc_server.hpp"

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
    exit(0); // server will not exit
    
    pthread_join(serverThread, &rcodePtr);
    printf("after joins \r");
}
int main( int argc, char* argv[] )
{
    test();
}
