#ifndef marvin_mitm_thread_hpp
#define marvin_mitm_thread_hpp

#include <string>
#include <vector>
#include <thread>
#include <boost/optional.hpp>

#include <marvin/server_v3/tcp_server.hpp>
#include <marvin/server_v3/mitm_app.hpp>
#include <marvin/collector/collector_base.hpp>
namespace Marvin {

class MitmThread
{
    public:
    MitmThread(
        boost::optional<long> proxy_port,
        boost::optional<std::string> marvin_home,
        boost::optional<std::vector<std::string>> https_regex,
        boost::optional<std::vector<std::string>> https_ports
    )
    {
        long port = (proxy_port) ? proxy_port.get() : 9992;

        std::vector<std::regex> re{std::regex("^ssllabs(.)*$")};
        std::vector<int> ports{443, 9443};
        MitmApp::configSet_HttpsPorts(ports);
        MitmApp::configSet_HttpsHosts(re);


        std::function<void(void*)> proxy_thread_func = [this, port](void* param) {
            m_server_uptr = std::make_unique<Marvin::TcpServer>([](boost::asio::io_service& io) {
                CollectorBaseSPtr cb_sptr = std::make_shared<CollectorBase>(io, std::cout);
                MitmAppUPtr app_uptr = std::make_unique<MitmApp>(io, cb_sptr);
                return app_uptr;
            });
            m_server_uptr->listen(port);
            std::cout << "Returned from listen" << std::endl;
        };
        m_thread_uptr = std::make_unique<std::thread>(proxy_thread_func, nullptr);

        // m_thread_uptr = std::make_unique<std::thread>([this]()
        // {
        //     while (true) {
        //         sleep(1);
        //         std::cout << __func__ << std::endl;
        //     }
        // });
    }
    ~MitmThread()
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
