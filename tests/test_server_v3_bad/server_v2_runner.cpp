#include <boost/asio.hpp>
#include "server_v2_runner.hpp"
#include "v2_handler.hpp"

static ServerRunnerV2SPtr runner_saved_sptr;

void startTestServer(long port)
{
    runner_saved_sptr = std::make_shared<ServerRunnerV2>();
    runner_saved_sptr->setup(port);
}
void stopTestServer()
{
    runner_saved_sptr->teardown();
}


ServerRunnerV2::ServerRunnerV2(){}
ServerRunnerV2::~ServerRunnerV2(){}

void ServerRunnerV2::setup(long port)
{
    this->server_thread_ptr = new std::thread([this, port](){
        std::cout << "Running server thread" << std::endl;
        try {
            Marvin::HttpServerV2* server_ptr = new Marvin::HttpServerV2([port](boost::asio::io_service& io){
                return new V2Handler(io);
            });
            this->server_ptr = server_ptr;
            server_ptr->listen(port);
        } catch(std::exception & ex) {
            throw ex;
            return;
        }
    });
}

void ServerRunnerV2::teardown()
{
    this->server_ptr->terminate();
    this->server_thread_ptr->join();
}
