#include <boost/asio.hpp>
#include "server_runner.hpp"
#include "tsc_req_handler.hpp"
ServerRunner::ServerRunner(){}
ServerRunner::~ServerRunner(){}

void ServerRunner::setup(long port)
{
    static std::thread* server_thread_ptr = new std::thread([this, port](){
        try {
            HTTPServer* server_ptr = new HTTPServer([port](boost::asio::io_service& io){
                return new TscRequestHandler(io);
            });
            this->server_ptr = server_ptr;
            server_ptr->listen(port);
            std::cout << __FUNCTION__ << " after listen" << std::endl;
        } catch(std::exception & ex) {
            return;
        }
    });
    this->server_thread_ptr = server_thread_ptr;
}

void ServerRunner::teardown()
{
    this->server_ptr->terminate();
    this->server_thread_ptr->join();
}
