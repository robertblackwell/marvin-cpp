#ifndef marvin_simple_proxy_ctl_thread_hpp
#define marvin_simple_proxy_ctl_thread_hpp

#include <string>
#include <vector>
#include <thread>
#include <boost/optional.hpp>
#include <marvin/collector/capture_filter.hpp>
#include <marvin/collector//capture_collector.hpp>
#include <marvin/server_v3/tcp_server.hpp>
#include "mitm_thread.hpp"

namespace Marvin {

class CtlApp;

typedef std::unique_ptr<CtlApp> CtlAppUPtr ;
typedef std::unique_ptr<CtlApp> CtlAppUPtr;

class CtlThread
{
    public:
    CtlThread(long ctl_port, long proxy_port, std::string marvin_home, CaptureFilter::SPtr capture_filter_sptr)
    : m_capture_filter_sptr(capture_filter_sptr), m_proxy_port(proxy_port), m_ctl_port(ctl_port)
    {

//        std::function<void(void*)> proxy_thread_func = [this, port](void* param) {
//            m_server_uptr = std::make_unique<Marvin::TcpServer>([this](boost::asio::io_service& io) {
//                CtlAppUPtr app_uptr = std::make_unique<CtlApp>(io, m_mitm_thread_ref, this);
//                return app_uptr;
//            });
//            m_server_uptr->listen(port);
//            std::cout << "CtlThread Returned from listen" << std::endl;
//        };
//        m_thread_uptr = std::make_unique<std::thread>(proxy_thread_func, nullptr);
    }
    ~CtlThread()
    {

    }
    void start()
    {
        std::function<void(void*)> proxy_thread_func = [this](void* param) {
            m_server_uptr = std::make_unique<Marvin::TcpServer>([this](boost::asio::io_service& io) {
                CtlAppUPtr app_uptr = std::make_unique<CtlApp>(io, m_mitm_thread_sptr, this);
                return app_uptr;
            });

            boost::asio::io_context& ctx = m_server_uptr->get_io_context();
            m_capture_collector_sptr = std::make_shared<CaptureCollector>(ctx, m_capture_filter_sptr);
            m_mitm_thread_sptr = std::make_shared<MitmThread>(m_proxy_port, m_marvin_home, m_capture_collector_sptr);
            m_mitm_thread_sptr->start();

            m_server_uptr->listen(m_ctl_port, [this]()
            {
                std::cout << __PRETTY_FUNCTION__ << " After listen" << std::endl;
                {
                    std::lock_guard<std::mutex> lck(this->m_mutex);
                    this->m_server_ready = true;
                }
                this->m_cond_var.notify_one();
                std::cout << __PRETTY_FUNCTION__ << " After notify" << std::endl;
            });

            std::cout << "CtlThread Returned from listen" << std::endl;
        };
        // wait for listen to invoke the callback
        {
            std::unique_lock<std::mutex> lck(this->m_mutex);
            this->m_cond_var.wait(lck, [this]{ return this->m_server_ready; });
        }

        m_thread_uptr = std::make_unique<std::thread>(proxy_thread_func, nullptr);
    }
    boost::asio::io_context& get_io_context()
    {
        return m_server_uptr->get_io_context();
    }
    void post(std::function<void()> f)
    {

    }

    void join()
    {
        m_mitm_thread_sptr->join();
        m_thread_uptr->join();
    }

    void terminate() {
        m_server_uptr->terminate();
    }
    // this is the link that allows the ctl thread and ctl_app to acces other parts of
    // the simple proxy
    MitmThread::SPtr             m_mitm_thread_sptr;
    CtlAppUPtr                   m_ctl_app_uptr;
    std::unique_ptr<TcpServer>   m_server_uptr;
    std::unique_ptr<std::thread> m_thread_uptr;
    long                         m_ctl_port;
    long                         m_proxy_port;
    std::string                  m_marvin_home;
    CaptureFilter::SPtr          m_capture_filter_sptr;
    CaptureCollector::SPtr       m_capture_collector_sptr;
    std::mutex 				    m_mutex;
    std::condition_variable     m_cond_var;
    bool                        m_server_ready;


};


class CtlApp : public RequestHandlerInterface
{
public:
    CtlApp(boost::asio::io_service& io,
        MitmThread::SPtr mitm_thread_sptr,
        CtlThread*  ctl_thread_ptr
    );
    ~CtlApp();

    void handle(
        ServerContext&            server_context,
        ISocketSPtr               connPtr,
        HandlerDoneCallbackType   done
    );

    typedef std::function<void(MessageReaderSPtr msg_rdr)> HttpRequestHandler; 
    struct DispatchItem {
        std::regex          pattern;
        HttpRequestHandler  handler;
    };
    struct DispatchTable {
        std::vector<DispatchItem> table;

        void add(std::regex apattern, HttpRequestHandler ahandler )
        {
            table.push_back({.pattern=apattern, ahandler});
        }
        boost::optional<HttpRequestHandler> find(std::string path) {
            for(auto item: table) {
                if (std::regex_match(path, item.pattern)) {
                    return item.handler;
                }
            }
            return boost::none;
        }
    };

    void p_req_resp_cycle_complete();
    void p_on_completed();
    void p_on_read_error(ErrorType err);
    void p_on_write_error(ErrorType err);


    void p_internal_handle();
    void p_handle_echo();
    void p_handle_stop(std::vector<std::string>& bits);
    void p_handle_list_filters(std::vector<std::string>& bits);
    void p_handle_smart_echo();
    void p_handle_delay(std::vector<std::string>& bits);
    void p_invalid_request();
    void p_non_specific_response();
    void p_echo_response();

    static int counter; // to see if there are multiple instances of the handler
    // boost::asio::deadline_timer m_imer;
    MitmThread::SPtr                    m_mitm_thread_sptr;
    CtlThread*                          m_ctl_thread_ptr;
    boost::uuids::uuid       m_uuid;
    boost::asio::io_service&            m_io;
    ISocketSPtr                         m_socket_sptr;
    MessageWriterSPtr                   m_wrtr;
    MessageReaderSPtr                   m_rdr;
    MessageBaseSPtr                     m_msg;
    std::string                         m_body;
    HandlerDoneCallbackType             m_done;
    ATimerSPtr                          m_timer_sptr;
    std::function<void()>               m_done_callback;

    DispatchTable                       m_dispatch_table;

};


}

#endif
