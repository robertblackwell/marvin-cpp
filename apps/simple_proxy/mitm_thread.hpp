#ifndef marvin_simple_proxy_mitm_thread_hpp
#define marvin_simple_proxy_mitm_thread_hpp

#include <string>
#include <vector>
#include <thread>
#include <boost/optional.hpp>

#include <marvin/server/tcp_server.hpp>
#include <marvin/server/mitm_app.hpp>
#include <marvin/collector/capture_filter.hpp>
#include <marvin/collector/cout_collector.hpp>
namespace Marvin {
namespace SimpleProxy {

struct MitmThread
{
    public:

    std::unique_ptr<TcpServer>   m_server_uptr;
    std::unique_ptr<std::thread> m_thread_uptr;
    CaptureFilter::SPtr          m_filter_sptr;
    CoutCollector::SPtr          m_collector_sptr;
    bool                         m_show_message_bodies;
    MitmThread()
    {

    }

    MitmThread(
        long                proxy_port,
        std::string         marvin_home,
        CaptureFilter::SPtr filter_sptr
    ): m_filter_sptr(filter_sptr)
    {
        long port = proxy_port;

        m_collector_sptr = nullptr;
        std::function<void(void*)> proxy_thread_func = [this, port](void* param) {

            m_server_uptr = std::make_unique<Marvin::TcpServer>([this](boost::asio::io_service& io) {
                if (m_collector_sptr == nullptr) {
                    m_collector_sptr = std::make_shared<CoutCollector>(io, m_filter_sptr);
                }
                MitmAppUPtr app_uptr = std::make_unique<MitmApp>(io, m_collector_sptr, m_filter_sptr);
                return app_uptr;
            });
            m_server_uptr->listen(port);
            std::cout << "Mitm Server returned from listen" << std::endl;
        };
        m_thread_uptr = std::make_unique<std::thread>(proxy_thread_func, nullptr);
    }
    ~MitmThread()
    {

    }
    
    void post(std::function<void()> f)
    {

    }

    void join() { 
        m_thread_uptr->join();
    }
    std::vector<std::string>& get_https_hosts() {
        return MitmApp::configGet_HttpsHosts();
    }
    std::vector<int>& get_https_ports()
    {
        static std::vector<int> x{};
        return x; //return MitmApp::configGet_HttpsPorts();
    }
    void terminate() {
        m_server_uptr->terminate();
    }


};
} // namespace SimpleProxy
} // namespace Marvin

#endif
