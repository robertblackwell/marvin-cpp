//
// Start both a server and a number of client tthreads and have each of the client
// threads make a number of requests from the server.
//
#ifndef marvin_test_server_v3_server_runner_hpp
#define marvin_test_server_v3_server_runner_hpp

#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <thread>
#include <marvin/boost_stuff.hpp>
#include <marvin/server_v3/tcp_server.hpp>
#if SERVER_RUNNER_STATIC
void startTestServer(long port=9991);
void stopTestServer();
#endif

using namespace Marvin;

class ServerRunner;
typedef std::shared_ptr<ServerRunner> ServerRunnerSPtr;

class ServerRunner
{
public:
    ServerRunner();
    ~ServerRunner();
    /* Start an instance of http_server on localhost:port with requests to be serviced by
    * handler instances created by the factory function.
    * 
    * @param long port Port to listen on
    * @param factory A function that creates an RequestHandler object to handle a single connection
    * 
    * Does not return until the server is fully operational
    */
    void setup(long port, RequestHandlerUPtrFactory factory);
    /* terminate the http_server previously started*/
    void waitForServerToComplete();
    void terminateServer();
    void teardown();

    void waitForServerToStart();
    TcpServer* 	                m_server_ptr;
    RequestHandlerUPtrFactory   m_factory;
    std::thread* 			    m_server_thread_ptr;
    std::mutex 				    m_mutex;
	std::condition_variable     m_cond_var; 
    bool                        m_server_ready;

};
#endif
