#ifndef marvin_test_server_v3_handler_hpp
#define marvin_test_server_v3_handler_hpp

#include <doctest/doctest.h>
#include <iostream>
#include <sstream>
#include <string>
#include <pthread.h>
#include <marvin/boost_stuff.hpp>
#include <marvin/external_src/rb_logger/rb_logger.hpp>
#include <marvin/http/http_header.hpp>
#include <marvin/connection/connection.hpp>
#include <marvin/http/uri_query.hpp>

#include <marvin/server_v3/http_server.hpp>
#include <marvin/server_v3/server_context.hpp>
#include <marvin/server_v3/request_handler_base.hpp>
#include "timer.hpp"
/**
* Class used to handle requests in the test server for both the bb abd bf
* set of tests. This handler:
*   -   json encodes the full request (headers and body)
*   -   json encodes the uuid for the connection handler and the request handler
*
* and puts that in the body of the response
*/
class Handler : public Marvin::RequestHandlerBase
{
public:
    static int counter; // to see if there are multiple instances of the handler
    // boost::asio::deadline_timer m_imer;
    boost::uuids::uuid       m_uuid;
    
    ISocketSPtr                         m_socket_sptr;
    MessageWriterSPtr                   m_wrtr;
    MessageReaderSPtr                   m_rdr;
    Marvin::Http::MessageBaseSPtr       m_msg;
    std::string                         m_body;
    Marvin::HandlerDoneCallbackType     m_done;
    ATimerSPtr                          m_timer_sptr;
    std::function<void()>               m_done_callback;



    Handler(boost::asio::io_service& io);
    ~Handler();
    
    void handle(
        Marvin::ServerContext&            server_context,
        ISocketSPtr                       connPtr,
        Marvin::HandlerDoneCallbackType   done
    );
    void p_internal_handle();
    void p_handle_echo();
    void p_handle_smart_echo();
    void p_handle_delay(std::vector<std::string>& bits);
    void p_invalid_request();
    void p_non_specific_response();
    void p_echo_response();
    void p_req_resp_cycle_complete();

    void p_on_read_error(Marvin::ErrorType err);
    void p_on_write_error(Marvin::ErrorType err);

};
#endif /* test_server_h */
