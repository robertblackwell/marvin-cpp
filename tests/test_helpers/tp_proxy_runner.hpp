#ifndef marvin_tp_proxy_runner_hpp
#define marvin_tp_proxy_runner_hpp
#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <thread>
#include <pthread.h>
#include <catch2/catch.hpp>
#include<marvin/boost_stuff.hpp>

#include <marvin/server/http_server.hpp>
#include <marvin/collector/pipe_collector.hpp>
#include <marvin/forwarding/forwarding_handler.hpp>

void startProxyServer(long port, std::string collector_fifo_name);
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
    std::thread* getThread() {return server_thread_ptr;}
};
typedef std::shared_ptr<ProxyRunner> ProxyRunnerSPtr;

#endif
