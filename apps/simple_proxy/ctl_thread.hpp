#ifndef marvin_simple_proxy_ctl_thread_hpp
#define marvin_simple_proxy_ctl_thread_hpp

#include <string>
#include <vector>
#include <thread>
#include <boost/optional.hpp>

#include <marvin/server/tcp_server.hpp>
#include "mitm_thread.hpp"

namespace Marvin {
namespace SimpleProxy {

class CtlApp;

typedef std::unique_ptr<CtlApp> CtlAppUPtr ;
typedef std::unique_ptr<CtlApp> CtlAppUPtr;

class CtlThread
{
    public:
    CtlThread(long ctl_port, MitmThread& mitm_thread_ref)
    : m_mitm_thread_ref(mitm_thread_ref)
    {
        long port = ctl_port;

        std::function<void(void*)> proxy_thread_func = [this, port](void* param) {
            m_server_uptr = std::make_unique<Marvin::TcpServer>([this](boost::asio::io_service& io) {
                CtlAppUPtr app_uptr = std::make_unique<CtlApp>(io, m_mitm_thread_ref, this);
                return app_uptr;
            });
            m_server_uptr->listen(port);
            std::cout << "CtlThread Returned from listen" << std::endl;
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
    // this is the link that allows the ctl thread and ctl_app to acces other parts of
    // the simple proxy
    MitmThread&                  m_mitm_thread_ref;
    CtlAppUPtr                   m_ctl_app_uptr;
    std::unique_ptr<TcpServer>   m_server_uptr;
    std::unique_ptr<std::thread> m_thread_uptr;

};


class CtlApp : public RequestHandlerInterface
{
public:
    CtlApp(boost::asio::io_service& io,
        MitmThread& mitm_thread_ref,
        CtlThread*  ctl_thread_ptr
    );
    ~CtlApp();

    void handle(
        ServerContext&            server_context,
        ISocketSPtr               connPtr,
        HandlerDoneCallbackType   done
    );

    typedef std::function<void(MessageBase::SPtr request)> HttpRequestHandler;
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
    void p_handle_filter(std::vector<std::string>& bits);
    void p_handle_bodies(std::vector<std::string>& bits);
    void p_handle_smart_echo();
    void p_handle_delay(std::vector<std::string>& bits);
    void p_invalid_request();
    void p_non_specific_response();
    void p_echo_response();

    static int counter; // to see if there are multiple instances of the handler
    // boost::asio::deadline_timer m_imer;
    MitmThread&                         m_mitm_thread_ref;
    CtlThread*                          m_ctl_thread_ptr;
    boost::uuids::uuid       m_uuid;
    boost::asio::io_service&            m_io;
    ISocketSPtr                         m_socket_sptr;
    MessageWriterSPtr                   m_wrtr;
    MessageReaderV2::SPtr               m_rdr;
    MessageBase::SPtr                   m_request_sptr;
    MessageBaseSPtr                     m_msg;
    std::string                         m_body;
    HandlerDoneCallbackType             m_done;
    ATimerSPtr                          m_timer_sptr;
    std::function<void()>               m_done_callback;

    DispatchTable                       m_dispatch_table;

};


} // namespace SimpleProxy
} // namespace Marvin
#endif
