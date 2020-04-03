#include <boost/asio.hpp>
#include "server_v3_runner.hpp"
#include "handler.hpp"
#include "handle_app.hpp"

#define SERVER_RUNNER_CVAR
ServerRunner::ServerRunner(){ m_server_ready = false;}
ServerRunner::~ServerRunner()
{
    std::cout << __PRETTY_FUNCTION__ << std::endl;
}

/* Uses a conditional variable and mutex to coordinate with the code that startes the server*/
void ServerRunner::waitForServerToStart()
{
    std::cout << __PRETTY_FUNCTION__ << " waiting for server" << std::endl;
    #ifdef SERVER_RUNNER_CVAR
    {
        std::unique_lock<std::mutex> lck(this->m_mutex);
        this->m_cond_var.wait(lck, [this]{ return this->m_server_ready; }); 
    }
    #endif
    std::cout << __PRETTY_FUNCTION__ << " server ready" << std::endl;
}
void ServerRunner::setup(long port, Marvin::RequestHandlerFactory factory)
{
    this->m_factory = factory;
    this->m_server_thread_ptr = new std::thread([this, port]()
    {
        std::cout << "Running marvin server on thread: " 
            << this->m_server_thread_ptr->get_id() << " on port: " << port << std::endl;
        
        m_server_ptr = new Marvin::HttpServer(m_factory);
        // m_server_ptr = new Marvin::HttpServer([port](boost::asio::io_service& io)
        // {
        //     return new Handler(io);
        // });
        m_server_ptr->listen(port, [this]()
        { 
            std::cout << __PRETTY_FUNCTION__ << " After listen" << std::endl;
            #ifdef SERVER_RUNNER_CVAR
            {
                std::lock_guard<std::mutex> lck(this->m_mutex);
                this->m_server_ready = true;
            }
            this->m_cond_var.notify_one();
            #endif
            std::cout << __PRETTY_FUNCTION__ << " After notify" << std::endl;
        });

    });
    this->waitForServerToStart();
}
void ServerRunner::waitForServerToComplete()
{
    this->m_server_thread_ptr->join();
}
void ServerRunner::terminateServer()
{
    this->m_server_ptr->terminate();
}
void ServerRunner::teardown()
{
    this->m_server_ptr->terminate();
    this->m_server_thread_ptr->join();
}
// std::mutex mutex_;
// std::condition_variable condVar; 

// bool dataReady{false};

// void waitingForWork(){
//     std::cout << "Waiting " << std::endl;
//     std::unique_lock<std::mutex> lck(mutex_);
//     condVar.wait(lck, []{ return dataReady; });   // (4)
//     std::cout << "Running " << std::endl;
// }

// void setDataReady(){
//     {
//         std::lock_guard<std::mutex> lck(mutex_);
//         dataReady = true;
//     }
//     std::cout << "Data prepared" << std::endl;
//     condVar.notify_one();                        // (3)
// }
