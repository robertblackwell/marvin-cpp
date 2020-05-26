#ifndef marvin_simple_proxy_mitm_thread_hpp
#define marvin_simple_proxy_mitm_thread_hpp

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
    using SPtr = std::shared_ptr<MitmThread>;

    MitmThread()
    {

    }
    MitmThread(
        long                   proxy_port,
        std::string            marvin_home,
        CaptureCollector::SPtr capture_collector_sptr
    ): m_capture_collector_sptr(capture_collector_sptr)
    {
        long port = proxy_port;
        m_port = port;
//        std::vector<std::string> re_strs{std::string("^ssllabs(.)*$")};
//        std::vector<int> ports{443, 9443};
//        MitmApp::configSet_HttpsPorts(ports);
//        MitmApp::configSet_HttpsHosts(re_strs);

//        std::function<void(void*)> proxy_thread_func = [this, port](void* param) {
//            m_server_uptr = std::make_unique<Marvin::TcpServer>([](boost::asio::io_service& io) {
//                CollectorBaseSPtr cb_sptr = std::make_shared<CollectorBase>(io, std::cout);
//                MitmAppUPtr app_uptr = std::make_unique<MitmApp>(io, cb_sptr);
//                return app_uptr;
//            });
//            m_server_uptr->listen(port);
//            std::cout << "Mitm Server returned from listen" << std::endl;
//        };
//        m_thread_uptr = std::make_unique<std::thread>(proxy_thread_func, nullptr);
    }
    ~MitmThread()
    {

    }
    /** does not return until listen is started*/
    void start()
    {
        std::function<void(void*)> proxy_thread_func = [this](void* param) {
            m_server_uptr = std::make_unique<Marvin::TcpServer>([this](boost::asio::io_service& io) {
                MitmAppUPtr app_uptr = std::make_unique<MitmApp>(io, m_capture_collector_sptr);
                return app_uptr;
            });
            m_server_uptr->listen(m_port, [this]()
            {
                std::cout << __PRETTY_FUNCTION__ << " After listen" << std::endl;
                {
                    std::lock_guard<std::mutex> lck(this->m_mutex);
                    this->m_server_ready = true;
                }
                this->m_cond_var.notify_one();
                std::cout << __PRETTY_FUNCTION__ << " After notify" << std::endl;
            });
            std::cout << "Mitm Server returned from listen" << std::endl;
        };
        {
            std::unique_lock<std::mutex> lck(this->m_mutex);
            this->m_cond_var.wait(lck, [this]{ return this->m_server_ready; });
        }
        m_thread_uptr = std::make_unique<std::thread>(proxy_thread_func, nullptr);
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

    long m_port;
    CaptureCollector::SPtr       m_capture_collector_sptr;
    std::unique_ptr<TcpServer>   m_server_uptr;
    std::unique_ptr<std::thread> m_thread_uptr;
    std::mutex 				    m_mutex;
    std::condition_variable     m_cond_var;
    bool                        m_server_ready;


};
}

#endif
