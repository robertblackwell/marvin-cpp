#ifndef marvin_tp_proxy_runner_hpp
#define marvin_tp_proxy_runner_hpp
#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <thread>
#include <pthread.h>
#include <catch/catch.hpp>
#include "boost_stuff.hpp"

#include "http_server.hpp"
#include "pipe_collector.hpp"
#include "forwarding_handler.hpp"

void startProxyServer(long port);
void stopProxyServer();

/**
* This produces a class starts and stops a specified server instance with the given
* request handler factory
*/
class ProxyRunner
{
public:
    HTTPServer* server_ptr;
    std::thread* server_thread_ptr;
//    ServerRunner();
//    ~ServerRunner();
    void setup(long port);
    void teardown();
};

#endif
