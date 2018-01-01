//#include <iostream>
//#include <sstream>
//#include <string>
//#include <unistd.h>
//#include <thread>
//#include <pthread.h>
//#include <gtest/gtest.h>
//#include "boost_stuff.hpp"
//
//#include "http_server.hpp"
//#include "pipe_collector.hpp"
//#include "forwarding_handler.hpp"
#include "tp_proxy_runner.hpp"
//ProxyRunner<Handler>::ServerRunner(){}
//
//ProxyRunner<Handler>::~ServerRunner(){}

void ProxyRunner::setup(long port)
{
    
    static std::thread* server_thread_ptr = new std::thread([this, port](){
        try {
            HTTPServer* server_ptr = new HTTPServer([](boost::asio::io_service& io){
                PipeCollector& pc = PipeCollector::getInstance(io);
                return new ForwardingHandler(io, pc);
            });
            this->server_ptr = server_ptr;
            server_ptr->listen(port);
            std::cout << __FUNCTION__ << " after listen port: " << port << std::endl;
        } catch(std::exception & ex) {
            return;
        }
    });
    this->server_thread_ptr = server_thread_ptr;
}

void ProxyRunner::teardown()
{
    this->server_ptr->terminate();
    this->server_thread_ptr->join();
}
