//
// Start both a server and a number of client tthreads and have each of the client
// threads make a number of requests from the server.
//
#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <thread>
#include <pthread.h>
#include <gtest/gtest.h>
#include "boost_stuff.hpp"

#include "http_server.hpp"
#include <gtest/gtest.h>
/**
* This produces a class starts and stops a specified server instance with the given
* request handler factory
*/
template<class Handler>
class ServerRunner
{
public:
    HTTPServer* server_ptr;
    std::thread* server_thread_ptr;
    ServerRunner();
    ~ServerRunner();
    void setup(long port);
    void teardown();
};

template<class Handler>
ServerRunner<Handler>::ServerRunner(){}

template<class Handler>
ServerRunner<Handler>::~ServerRunner(){}

template<class Handler>
void ServerRunner<Handler>::setup(long port)
{
    static std::thread* server_thread_ptr = new std::thread([this, port](){
        try {
            HTTPServer* server_ptr = new HTTPServer([](boost::asio::io_service& io){
                return new Handler(io);
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

template<class Handler>
void ServerRunner<Handler>::teardown()
{
    this->server_ptr->terminate();
    this->server_thread_ptr->join();
}
