#ifndef marvin_simple_proxy_ctl_thread_hpp
#define marvin_simple_proxy_ctl_thread_hpp

#include <string>
#include <vector>
#include <thread>
#include <boost/optional.hpp>

#include <marvin/server_v3/tcp_server.hpp>

#include "ctl_app.hpp"

namespace Marvin {

class CtlThread
{
    public:
    CtlThread(boost::optional<long> ctl_port)
    {
        long port = (ctl_port) ? ctl_port.get() : 9993;

        std::function<void(void*)> proxy_thread_func = [this, port](void* param) {
            m_server_uptr = std::make_unique<Marvin::TcpServer>([](boost::asio::io_service& io) {
                CtlAppUPtr app_uptr = std::make_unique<CtlApp>(io);
                return app_uptr;
            });
            m_server_uptr->listen(port);
            std::cout << "Returned from listen" << std::endl;
        };
        m_thread_uptr = std::make_unique<std::thread>(proxy_thread_func, nullptr);

    }
    ~CtlThread()
    {

    }
    
    void post(std::function<void()> f) {

    }

    void join() { 
        m_thread_uptr->join();
    }

    void terminate() {
        m_server_uptr->terminate();
    }

    std::unique_ptr<TcpServer>   m_server_uptr;
    std::unique_ptr<std::thread> m_thread_uptr;

};
}

#endif
