#include <boost/asio.hpp>
#include "server_runner.hpp"
#include "./handler.hpp"

static std::shared_ptr<ServerRunner> server_runner_sptr;
void startTestServer(long port)
{
    server_runner_sptr = std::make_shared<ServerRunner>();
    server_runner_sptr->setup(port);
}
void stopTestServer()
{
    server_runner_sptr->teardown();
}


ServerRunner::ServerRunner(){}
ServerRunner::~ServerRunner(){}

void ServerRunner::setup(long port)
{
    this->server_thread_ptr = new std::thread([this, port](){
        std::cout << __PRETTY_FUNCTION__ << std::endl;
        try {
            HTTPServer* server_ptr = new HTTPServer([port](boost::asio::io_service& io){
                return new V1Handler(io);
            });
            this->server_ptr = server_ptr;
            server_ptr->listen(port);
        } catch(std::exception & ex) {
            throw ex;
            return;
        }
    });
    std::cout << __PRETTY_FUNCTION__ << std::endl;
}

void ServerRunner::teardown()
{
    this->server_ptr->terminate();
    this->server_thread_ptr->join();
}
