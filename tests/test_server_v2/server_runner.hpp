//
// Start both a server and a number of client tthreads and have each of the client
// threads make a number of requests from the server.
//
#ifndef marvin_test_server_v2_runner_hpp
#define marvin_test_server_v2_runner_hpp
#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <thread>
#include <marvin/boost_stuff.hpp>
#include <marvin/server_v2/http_server.hpp>
void startTestServer(long port=9991);
void stopTestServer();

using namespace V2;
/*
* This class starts and stops a server with the given
* request handler factory
*/
class ServerRunner
{
public:
    HttpServer* server_ptr;
    std::thread* server_thread_ptr;
    ServerRunner();
    ~ServerRunner();
    void setup(long port);
    void teardown();
};
typedef std::shared_ptr<ServerRunner> ServerRunnerSPtr;
#endif
