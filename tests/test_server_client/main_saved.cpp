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
#include <marvin/configure_trog.hpp>
TROG_SET_FILE_LEVEL(Trog::LogLevelWarn)
#include <marvin/server/http_server.hpp>
#include <marvin/server/request_handler_base.hpp>
#include "tsc_client.hpp"
#include "tsc_server.hpp"
void*
serverFunction(void* arg)
{
    long tid;
    tid = (long)arg;
     printf("Hello World! This is server\n");
    runTestServer();
    printf("server complete\n");

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
#define NO_GOOGLE 
#ifdef NO_GOOGLE
int main()
{
    std::cout.setf(std::ios::unitbuf);
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    
    int rc;
    long t;
    pthread_t client_thread, server_thread;
    int rc_s = pthread_create(&server_thread, &attr, serverFunction, (void*)t);
    int rc_c = pthread_create(&server_thread, &attr, clientFunction, (void*)t);
    void* status;
    int rc_j = pthread_join(client_thread, &status);
    int rc_j2 = pthread_join(server_thread, &status);
//    tsc_server_stop();
    return 0;
//    tsc_server_stop();
//    server_thread.join();
    printf("after joins \r");
}
//int main( int argc, char* argv[] )
//{
//    test();
//}
#else
TEST(ServerClient, Test)
{
    test();
}

#pragma mark - main
int main(int argc, char * argv[]) {
    RBLogging::setEnabled(false);
    BufferChain chain;
    char* _argv[2] = {argv[0], (char*)"--gtest_filter=*.*"}; // change the filter to restrict the tests that are executed
    int _argc = 2;
    testing::InitGoogleTest(&_argc, _argv);
    printf("Progrm exit");
    return RUN_ALL_TESTS();
}
#endif
