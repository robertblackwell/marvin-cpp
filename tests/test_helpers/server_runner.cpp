#include <boost/asio.hpp>
#include "server_runner.hpp"
#include "tsc_req_handler.hpp"

static std::shared_ptr<ServerRunner> serverRunnerSPtr;
void startTestServer(long port)
{
    serverRunnerSPtr = std::make_shared<ServerRunner>();
    serverRunnerSPtr->setup(port);
}
void stopTestServer()
{
    serverRunnerSPtr->teardown();
}


ServerRunner::ServerRunner(){}
ServerRunner::~ServerRunner(){}

void ServerRunner::setup(long port)
{
    this->server_thread_ptr = new std::thread([this, port](){
        try {
            HTTPServer* server_ptr = new HTTPServer([port](boost::asio::io_service& io){
                return new TscRequestHandler(io);
            });
            this->server_ptr = server_ptr;
            server_ptr->listen(port);
        } catch(std::exception & ex) {
            throw ex;
            return;
        }
    });
}

void ServerRunner::teardown()
{
    this->server_ptr->terminate();
    this->server_thread_ptr->join();
}
