//#include <iostream>
//#include <sstream>
//#include <string>
//#include <unistd.h>
//#include <thread>
//#include <pthread.h>
//#include <catch2/catch.h>
//#include "boost_stuff.hpp"
//
//#include "http_server.hpp"
//#include "pipe_collector.hpp"
//#include "forwarding_handler.hpp"
#include "tp_proxy_runner.hpp"
//ProxyRunner<Handler>::ServerRunner(){}
//
//ProxyRunner<Handler>::~ServerRunner(){}
static std::shared_ptr<ProxyRunner> proxyRunnerSPtr;
void startProxyServer(long port, std::string collector_fifo_name)
{
    PipeCollector::configSet_PipePath(collector_fifo_name);

    std::vector<std::regex> re{std::regex("^ssllabs(.)*$")};
    std::vector<int> ports{443, 9443};
    ForwardingHandler::configSet_HttpsPorts(ports);
    ForwardingHandler::configSet_HttpsHosts(re);
    proxyRunnerSPtr = std::make_shared<ProxyRunner>();
    proxyRunnerSPtr->setup(port);
}
void stopProxyServer()
{
    proxyRunnerSPtr->teardown();
}

void ProxyRunner::setup(long port)
{
    
    this->server_thread_ptr = new std::thread([this, port](){
        try {
            HTTPServer* server_ptr = new HTTPServer([](boost::asio::io_service& io){
                std::cout << "lambda creating HttpServer" << std::endl;
                PipeCollector* pc = PipeCollector::getInstance(io);
                std::cout << "lambda creating HttpServer after pipe collector getInstance" << std::endl;
                auto f = new ForwardingHandler(io, pc);
                std::cout << "lambda creating HttpServer after create forwarding handler" << std::endl;
                return f;
            });
            this->server_ptr = server_ptr;
            server_ptr->listen(port);
        } catch(std::exception & ex) {
            std::cout << "HttpServer start failed " << ex.what() << std::endl;
            assert(false);
            return;
        }
    });
}

void ProxyRunner::teardown()
{
//    this->server_ptr->terminate();
    this->server_thread_ptr->join();
}
