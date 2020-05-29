#ifndef marvin_proxy_app_mitm_thread_hpp
#define marvin_proxy_app_mitm_thread_hpp

#include <string>
#include <vector>
#include <thread>
#include <boost/optional.hpp>

#include <marvin/server_v3/tcp_server.hpp>
#include <marvin/server_v3/mitm_app.hpp>
#include <marvin/collector/collector_base.hpp>
#include <marvin/collector/capture_filter.hpp>
#include <marvin/collector//capture_collector.hpp>
namespace Marvin {

class MitmThread
{
    public:
    MitmThread()
    {

    }
    MitmThread(
        long                   proxy_port,
        std::string            marvin_home,
        CaptureFilter::SPtr    capture_filter_sptr,
        CaptureCollector::SPtr capture_collector_sptr
    ): m_proxy_port(proxy_port), m_capture_filter_sptr(capture_filter_sptr), m_capture_collector_sptr(capture_collector_sptr)
    {
    }
    void start()
    {
        m_server_ready = false;
        std::function<void(void*)> proxy_thread_func = [this](void* param)
        {
            m_server_uptr = std::make_unique<Marvin::TcpServer>([this](boost::asio::io_service& io)
            {
                MitmAppUPtr app_uptr = std::make_unique<MitmApp>(io, m_capture_collector_sptr);
                return app_uptr;
            });
            m_server_uptr->listen(m_proxy_port, [this]()
            {
                std::cout << __PRETTY_FUNCTION__ << " After listen" << std::endl;
                {
                std::lock_guard<std::mutex> lck(this->m_mutex);
                this->m_server_ready = true;
                }
                this->m_cond_var.notify_all();
                std::cout << __PRETTY_FUNCTION__ << " After notify" << std::endl;
            });
        };
        m_thread_uptr = std::make_unique<std::thread>(proxy_thread_func, nullptr);
        // wait for server to complete startup
        {
            std::unique_lock<std::mutex> lck(this->m_mutex);
            this->m_cond_var.wait(lck, [this]{ return this->m_server_ready; });
        }
        std::cout << "Mitm Server returned from listen" << std::endl;
    }
    ~MitmThread()
    {

    }
    
    void post(std::function<void()> f) {

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

    std::unique_ptr<TcpServer>   m_server_uptr;
    std::unique_ptr<std::thread> m_thread_uptr;
    long                         m_proxy_port;
    CaptureFilter::SPtr          m_capture_filter_sptr;
    CaptureCollector::SPtr       m_capture_collector_sptr;
    std::mutex 				     m_mutex;
    std::condition_variable      m_cond_var;
    bool                         m_server_ready;


};
}

#endif
