#ifndef marvin_test_server_v3_handle_app_hpp
#define marvin_test_server_v3_handle_app_hpp

#include <doctest/doctest.h>
#include <iostream>
#include <sstream>
#include <string>
#include <pthread.h>
#include <marvin/boost_stuff.hpp>
#include <marvin/http/header_fields.hpp>
#include <marvin/connection/connection.hpp>
#include <marvin/http/uri_query.hpp>

#include <marvin/server_v3/tcp_server.hpp>
#include <marvin/server_v3/server_context.hpp>
#include <marvin/server_v3/request_handler_interface.hpp>
#include <marvin/server_v3/timer.hpp>
using namespace Marvin;

class AppHandler : public RequestHandlerInterface
{
public:
    AppHandler(boost::asio::io_service& io);
    ~AppHandler();

    void handle(
        ServerContext&            server_context,
        ISocketSPtr               connPtr,
        HandlerDoneCallbackType   done
    );

    void p_req_resp_cycle_complete();
    void p_on_completed();
    void p_on_read_error(ErrorType err);
    void p_on_write_error(ErrorType err);


    void p_internal_handle();
    void p_handle_echo();
    void p_handle_smart_echo();
    void p_handle_delay(std::vector<std::string>& bits);
    void p_invalid_request();
    void p_non_specific_response();
    void p_echo_response();

    static int counter; // to see if there are multiple instances of the handler
    // boost::asio::deadline_timer m_imer;
    boost::uuids::uuid       m_uuid;
    boost::asio::io_service&            m_io;
    ISocketSPtr                         m_socket_sptr;
    MessageWriter::SPtr                 m_wrtr;
    MessageReaderV2::SPtr               m_rdr;
    MessageBase::SPtr                   m_request_sptr;
    MessageBase::SPtr                   m_msg;
    std::string                         m_body;
    HandlerDoneCallbackType             m_done;
    ATimerSPtr                          m_timer_sptr;
    std::function<void()>               m_done_callback;


};
#endif /* test_server_h */
